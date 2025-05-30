/** @file
	Parser: @b status class impl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>

	Win32 rusage author: Victor Fedoseev <vvf_ru@mail.ru>
*/

#include "pa_vstatus.h"
#include "pa_cache_managers.h"
#include "pa_vhash.h"
#include "pa_vdouble.h"
#include "pa_vstring.h"
#include "pa_threads.h"

volatile const char * IDENT_PA_VSTATUS_C="$Id: pa_vstatus.C,v 1.46 2025/05/26 00:52:15 moko Exp $" IDENT_PA_VSTATUS_H;

#ifdef _MSC_VER
#include <windows.h>
#include "psapi.h"

// should be in windows.h, but were't
typedef struct _IO_COUNTERS_ {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS_;
typedef IO_COUNTERS_ *PIO_COUNTERS_;

typedef unsigned __int64 ui64;
// kernel32.dll (NT/2K/XP)
typedef BOOL (WINAPI *PGETPROCESSTIMES)(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
//typedef BOOL (WINAPI *PGETPROCESSHEAPS)(DWORD,PHANDLE);
typedef BOOL (WINAPI *GETPROCESSIOCOUNTERS)(HANDLE,PIO_COUNTERS_);
// psapi.dll (2K/XP)
typedef BOOL (WINAPI *PGETPROCESSMEMORYINFO)(HANDLE,PPROCESS_MEMORY_COUNTERS,DWORD);

// from CRT time.c
/*
 * Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
 */
#define EPOCH_BIAS  116444736000000000i64

/*
 * Union to facilitate converting from FILETIME to unsigned __int64
 */
typedef union {
	unsigned __int64 ft_scalar;
	FILETIME ft_struct;
} FT;

int gettimeofday(struct timeval *tv, void *) {
	FT ft;
	GetSystemTimeAsFileTime( &(ft.ft_struct) );
	ft.ft_scalar -= EPOCH_BIAS;
	tv->tv_sec = (long)(ft.ft_scalar/10000000i64);
	tv->tv_usec = (long)((ft.ft_scalar- tv->tv_sec*10000000i64)/10i64);
	return 0;
}

#endif

Value& rusage_element() {
	VHash& rusage=*new VHash;
	HashStringValue& hash=rusage.hash();

#ifdef _MSC_VER
	double d1;
	HANDLE hProc = GetCurrentProcess();

	HMODULE hMod = LoadLibrary("kernel32.dll");
	if(hMod){
		// NT/2K/XP
		PGETPROCESSTIMES pGetProcessTimes = (PGETPROCESSTIMES)GetProcAddress(hMod, "GetProcessTimes");
		if(pGetProcessTimes){
			FILETIME CreationTime, ExitTime;
			FT KernelTime, UserTime;
			if(pGetProcessTimes(hProc, &CreationTime, &ExitTime, &KernelTime.ft_struct, &UserTime.ft_struct)){
				// dwHighDateTime & dwLowDateTime - 1/10 000 000 seconds in 64 bit
				/* the amount of time that the process has executed in user mode */
				d1 = double((LONGLONG)UserTime.ft_scalar)/10000000.0;
				HASH_PUT_CSTR(hash, "utime", new VDouble(d1));
				
				/* the amount of time that the process has executed in kernel mode */
				d1 = double((LONGLONG)KernelTime.ft_scalar)/10000000.0;
				HASH_PUT_CSTR(hash, "stime", new VDouble(d1));
			}
		}

		// NT/2K/XP
		GETPROCESSIOCOUNTERS pGetProcessIoCounters = (GETPROCESSIOCOUNTERS)GetProcAddress(hMod, "GetProcessIoCounters");
		if(pGetProcessIoCounters){
			IO_COUNTERS_ ioc;
			if(pGetProcessIoCounters(hProc, &ioc)){
				/* Specifies the number of I/O operations performed, other than read and write operations */
				HASH_PUT_CSTR(hash, "OtherOperationCount", new VDouble(double((LONGLONG)ioc.OtherOperationCount)));
				/* Specifies the number of bytes transferred during operations other than read and write operations */
				HASH_PUT_CSTR(hash, "OtherTransferCount", new VDouble(double((LONGLONG)ioc.OtherTransferCount)/1024.0));
				/* Specifies the number of read operations performed */
				HASH_PUT_CSTR(hash, "ReadOperationCount", new VDouble(double((LONGLONG)ioc.ReadOperationCount)));
				/* Specifies the number of bytes read */
				HASH_PUT_CSTR(hash, "ReadTransferCount", new VDouble(double((LONGLONG)ioc.ReadTransferCount)/1024.0));
				/* Specifies the number of write operations performed */
				HASH_PUT_CSTR(hash, "WriteOperationCount", new VDouble(double((LONGLONG)ioc.WriteOperationCount)));
				/* Specifies the number of bytes written */
				HASH_PUT_CSTR(hash, "WriteTransferCount", new VDouble(double((LONGLONG)ioc.WriteTransferCount)/1024.0));
			}
		}
		FreeLibrary(hMod);
		/*
		PGETPROCESSHEAPS pGetProcessHeaps = (PGETPROCESSHEAPS)GetProcAddress(hMod, "GetProcessHeaps");
		if(pGetProcessHeaps){
		}
		*/
	}

	// 2K/XP
	hMod = LoadLibrary("psapi.dll");
	if(hMod){
		PGETPROCESSMEMORYINFO pGetProcessMemoryInfo = (PGETPROCESSMEMORYINFO)GetProcAddress(hMod, "GetProcessMemoryInfo");
		if(pGetProcessMemoryInfo){
			PROCESS_MEMORY_COUNTERS pmc;
			pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);
			if(pGetProcessMemoryInfo(hProc, &pmc, sizeof(PROCESS_MEMORY_COUNTERS))){
				/* The peak working set size */
				d1 = double(pmc.PeakWorkingSetSize)/1024.0;
				HASH_PUT_CSTR(hash, "maxrss", new VDouble(d1));
				/* The peak nonpaged pool usage */
				d1 = double(pmc.QuotaPeakNonPagedPoolUsage)/1024.0;
				HASH_PUT_CSTR(hash, "QuotaPeakNonPagedPoolUsage", new VDouble(d1));
				/* The peak paged pool usage */
				d1 = double(pmc.QuotaPeakPagedPoolUsage)/1024.0;
				HASH_PUT_CSTR(hash, "QuotaPeakPagedPoolUsage", new VDouble(d1));
				/* The peak pagefile usage */
				d1 = double(pmc.PeakPagefileUsage)/1024.0;
				HASH_PUT_CSTR(hash, "PeakPagefileUsage", new VDouble(d1));
			}
		}
		FreeLibrary(hMod);
	}

#else

#ifdef HAVE_GETRUSAGE
    struct rusage u;
    if(getrusage(RUSAGE_SELF,&u)<0)
		throw Exception(0, 0, "getrusage failed (#%d)", errno);

	HASH_PUT_CSTR(hash, "utime", new VDouble(u.ru_utime.tv_sec+u.ru_utime.tv_usec/1000000.0));
	HASH_PUT_CSTR(hash, "stime", new VDouble(u.ru_stime.tv_sec+u.ru_stime.tv_usec/1000000.0));
	HASH_PUT_CSTR(hash, "maxrss", new VDouble(u.ru_maxrss));
	HASH_PUT_CSTR(hash, "ixrss", new VDouble(u.ru_ixrss));
	HASH_PUT_CSTR(hash, "idrss", new VDouble(u.ru_idrss));
	HASH_PUT_CSTR(hash, "isrss", new VDouble(u.ru_isrss));
#endif

#endif

	struct timeval tp;
	if(gettimeofday(&tp, NULL)<0)
		throw Exception(0, 0, "gettimeofday failed (#%d)", errno);

	HASH_PUT_CSTR(hash, "tv_sec", new VDouble(tp.tv_sec));
	HASH_PUT_CSTR(hash, "tv_usec", new VDouble(tp.tv_usec));

	return rusage;
}

#ifndef PA_DEBUG_DISABLE_GC
Value& memory_element() {
	VHash& memory=*new VHash;
	HashStringValue& hash=memory.hash();
	size_t heap_size=GC_get_heap_size();
	size_t free_bytes=GC_get_free_bytes();
	size_t bytes_since_gc=GC_get_bytes_since_gc();
	size_t total_bytes=GC_get_total_bytes();

	HASH_PUT_CSTR(hash, "used", new VDouble((heap_size-free_bytes)/1024.0));
	HASH_PUT_CSTR(hash, "free", new VDouble(free_bytes/1024.0));
	HASH_PUT_CSTR(hash, "ever_allocated_since_compact", new VDouble(bytes_since_gc/1024.0));
	HASH_PUT_CSTR(hash, "ever_allocated_since_start", new VDouble(total_bytes/1024.0));

	return memory;
}
#endif

extern const char* parser3_mode;
const char *parser3_log_filespec();

Value* VStatus::get_element(const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// CLASS, CLASS_NAME
	if(Value* result=VStateless_class::get_element(aname))
		return result;
#endif

	// getstatus
	if(Cache_manager* manager=cache_managers->get(aname))
		return manager->get_status();

	if(aname=="pid")
		return new VInt(getpid());

	if(aname=="tid")
		return new VInt(pa_get_thread_id());

	if(aname=="mode")
		return new VString(*new String(parser3_mode));

	if(aname=="log-filename")
		return new VString(*new String(pa_strdup(parser3_log_filespec())));

	if(aname=="rusage")
		return &rusage_element();

#ifndef PA_DEBUG_DISABLE_GC
	if(aname=="memory")
		return &memory_element();
#endif

	return 0;
}
