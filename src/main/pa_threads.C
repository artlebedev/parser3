/*
  $Id: pa_threads.C,v 1.1 2001/01/29 11:17:50 paf Exp $
*/

#include "pa_threads.h"

#ifdef MULTITHREAD

// either apache module or win32 prog for now
#ifdef MOD_PARSER
#	include "ap_config.h"
#else// WIN32
#	include <winbase.h>
#endif


#ifdef MOD_PARSER

Mutex::Mutex() {
	handle=reinterpret_cast<mutex *>(ap_create_mutex(0));
}

Mutex::~Mutex() {
	ap_destroy_mutex(reinterpret_cast<mutex *>(handle));
}

void Mutex::lock() {
	ap_acquire_mutex(reinterpret_cast<mutex *>(handle));
}

void Mutex::unlock() {
	ap_release_mutex(reinterpret_cast<mutex *>(handle));
}

#else// WIN32

Mutex::Mutex() {
	handle=CreateMutex(NULL, FALSE, 0);
}

Mutex::~Mutex() {
	CloseHandle(handle);
}

void Mutex::lock() {
	WaitForSingleObject(handle, INFINITE);
}

void Mutex::unlock() {
	ReleaseMutex(handle);
}

#endif

#endif
