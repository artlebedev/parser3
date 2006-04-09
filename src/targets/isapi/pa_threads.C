/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_THREADS_C="$Date: 2006/04/09 13:38:47 $";

#include "pa_threads.h"

#define  WINVER  0x0400
#include <windows.h>

const bool parser_multithreaded=true;

pa_thread_t pa_get_thread_id() {
	return GetCurrentThreadId();
}

Mutex global_mutex;

Mutex::Mutex() :
	handle(reinterpret_cast<uint>(CreateMutex(NULL, FALSE, 0))) {
}

Mutex::~Mutex() {
	CloseHandle(reinterpret_cast<HANDLE>(handle));
}

void Mutex::acquire() {
    WaitForSingleObject(reinterpret_cast<HANDLE>(handle), INFINITE);
}

void Mutex::release() {
    ReleaseMutex(reinterpret_cast<HANDLE>(handle));
}
