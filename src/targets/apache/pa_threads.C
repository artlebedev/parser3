/** @file
	Parser: Mutex realization class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_threads.h"

volatile const char * IDENT_PA_THREADS_C="$Id: pa_threads.C,v 1.2 2012/03/16 09:24:15 moko Exp $" IDENT_PA_THREADS_H;

Mutex global_mutex;

#ifdef WIN32

#define  WINVER  0x0400
#include <windows.h>

const bool parser_multithreaded=true;

pa_thread_t pa_get_thread_id() {
	return GetCurrentThreadId();
}

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

#else

const bool parser_multithreaded=false;

pa_thread_t pa_get_thread_id() { return 1; }

Mutex::Mutex() {}
Mutex::~Mutex() {}
void Mutex::acquire() {}
void Mutex::release() {}

#endif
