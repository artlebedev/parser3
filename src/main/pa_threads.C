/*
  $Id: pa_threads.C,v 1.2 2001/01/29 11:53:42 paf Exp $
*/

#include "pa_threads.h"

#ifdef MULTITHREAD

Mutex global_mutex;

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

void Mutex::acquire() {
	ap_acquire_mutex(reinterpret_cast<mutex *>(handle));
}

void Mutex::release() {
	ap_release_mutex(reinterpret_cast<mutex *>(handle));
}

#else// WIN32

Mutex::Mutex() {
	handle=CreateMutex(NULL, FALSE, 0);
}

Mutex::~Mutex() {
	CloseHandle(handle);
}

void Mutex::acquire() {
	WaitForSingleObject(handle, INFINITE);
}

void Mutex::release() {
	ReleaseMutex(handle);
}

#endif

#endif
