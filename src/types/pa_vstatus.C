/** @file
	Parser: @b status class impl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	Win32 rusage author: Victor Fedoseev <vvf_ru@mail.ru>
*/

static const char* IDENT_VSTATUS_C="$Date: 2003/03/13 08:03:15 $";

#include "pa_vstatus.h"
#include "pa_cache_managers.h"
#include "pa_vhash.h"
#include "pa_vdouble.h"

#ifdef HAVE_SYS_RESOURCE_H
// rusage
#include <sys/resource.h>
#endif

#ifdef WIN32
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

#endif

Value *VStatus::get_element(const String& aname, Value& /*aself*/, bool /*looking_up*/) {
	// getstatus
	if(Cache_manager *manager=static_cast<Cache_manager *>(cache_managers->get(aname)))
		return &manager->get_status(pool(), &aname);

	// rusage
	if(aname=="rusage") {
		VHash& rusage=*NEW VHash(pool());
		Hash& hash=rusage.hash(0);

#ifdef WIN32
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
					hash.put(*NEW String(pool(), "utime"),  NEW VDouble(pool(), d1));
//					hash.put(*NEW String(pool(), "UserTime"),  NEW VDouble(pool(),  d1));
					
					/* the amount of time that the process has executed in kernel mode */
					d1 = double((LONGLONG)KernelTime.ft_scalar)/10000000.0;
					hash.put(*NEW String(pool(), "stime"),  NEW VDouble(pool(), d1));
//					hash.put(*NEW String(pool(), "KernelTime"),  NEW VDouble(pool(),  d1));
				}
			}

			// NT/2K/XP
			GETPROCESSIOCOUNTERS pGetProcessIoCounters = (GETPROCESSIOCOUNTERS)GetProcAddress(hMod, "GetProcessIoCounters");
			if(pGetProcessIoCounters){
				IO_COUNTERS_ ioc;
				if(pGetProcessIoCounters(hProc, &ioc)){
					/* Specifies the number of I/O operations performed, other than read and write operations */
					hash.put(*NEW String(pool(),      "OtherOperationCount"),
						NEW VDouble(pool(), double((LONGLONG)ioc.OtherOperationCount)));
					/* Specifies the number of bytes transferred during operations other than read and write operations */
					hash.put(*NEW String(pool(),      "OtherTransferCount"),
						NEW VDouble(pool(), double((LONGLONG)ioc.OtherTransferCount)/1024.0));
					/* Specifies the number of read operations performed */
					hash.put(*NEW String(pool(),      "ReadOperationCount"),
						NEW VDouble(pool(), double((LONGLONG)ioc.ReadOperationCount)));
					/* Specifies the number of bytes read */
					hash.put(*NEW String(pool(),      "ReadTransferCount"),
						NEW VDouble(pool(), double((LONGLONG)ioc.ReadTransferCount)/1024.0));
					/* Specifies the number of write operations performed */
					hash.put(*NEW String(pool(),      "WriteOperationCount"),
						NEW VDouble(pool(), double((LONGLONG)ioc.WriteOperationCount)));
					/* Specifies the number of bytes written */
					hash.put(*NEW String(pool(),      "WriteTransferCount"),
						NEW VDouble(pool(), double((LONGLONG)ioc.WriteTransferCount)/1024.0));
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
					hash.put(*NEW String(pool(), "maxrss"), NEW VDouble(pool(), d1));
//					hash.put(*NEW String(pool(), "PeakWorkingSetSize"),  NEW VDouble(pool(),  d1));
					/* The peak nonpaged pool usage */
					d1 = double(pmc.QuotaPeakNonPagedPoolUsage)/1024.0;
//					hash.put(*NEW String(pool(), "ixrss"), NEW VDouble(pool(), d1));
					hash.put(*NEW String(pool(), "QuotaPeakNonPagedPoolUsage"),  NEW VDouble(pool(),  d1));
					/* The peak paged pool usage */
					d1 = double(pmc.QuotaPeakPagedPoolUsage)/1024.0;
//					hash.put(*NEW String(pool(), "idrss"),  NEW VDouble(pool(),  d1));
					hash.put(*NEW String(pool(), "QuotaPeakPagedPoolUsage"),  NEW VDouble(pool(),  d1));
					/* The peak pagefile usage */
					d1 = double(pmc.PeakPagefileUsage)/1024.0;
//					hash.put(*NEW String(pool(), "isrss"),  NEW VDouble(pool(),  d1));
					hash.put(*NEW String(pool(), "PeakPagefileUsage"),  NEW VDouble(pool(),  d1));
				}
			}
			FreeLibrary(hMod);
		}

		// all windows.
		FT ft;
        GetSystemTimeAsFileTime( &(ft.ft_struct) );
		ft.ft_scalar -= EPOCH_BIAS;
		ui64 tv_sec = ft.ft_scalar/10000000i64;
		ui64 tv_usec = (ft.ft_scalar-tv_sec*10000000i64)/10i64;
		hash.put(*NEW String(pool(), "tv_sec"), NEW VDouble(pool(), double((LONGLONG)tv_sec)));
		hash.put(*NEW String(pool(), "tv_usec"), NEW VDouble(pool(), double((LONGLONG)tv_usec)));

#else

#ifdef HAVE_GETRUSAGE
	    struct rusage u;
	    if(getrusage(RUSAGE_SELF,&u)<0)
			throw Exception(0,
				&aname,
				"getrusage failed (#%d)", errno);

		hash.put(*NEW String(pool(), "utime"), NEW VDouble(pool(), 
			u.ru_utime.tv_sec+u.ru_utime.tv_usec/1000000.0));
		hash.put(*NEW String(pool(), "stime"), NEW VDouble(pool(), 
			u.ru_stime.tv_sec+u.ru_stime.tv_usec/1000000.0));
		hash.put(*NEW String(pool(), "maxrss"), NEW VDouble(pool(), u.ru_maxrss));
		hash.put(*NEW String(pool(), "ixrss"), NEW VDouble(pool(), u.ru_ixrss));
		hash.put(*NEW String(pool(), "idrss"), NEW VDouble(pool(), u.ru_idrss));
		hash.put(*NEW String(pool(), "isrss"), NEW VDouble(pool(), u.ru_isrss));
#endif

#ifdef HAVE_GETTIMEOFDAY
		struct timeval tp;
		if(gettimeofday(&tp, NULL)<0)
			throw Exception(0,
				&aname,
				"gettimeofday failed (#%d)", errno);

		hash.put(*NEW String(pool(), "tv_sec"), NEW VDouble(pool(), tp.tv_sec));
		hash.put(*NEW String(pool(), "tv_usec"), NEW VDouble(pool(), tp.tv_usec));
#endif

#endif

		return &rusage;
	}

	return 0;
}
