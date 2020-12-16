/** @file
	Parser: simple Mutex realization class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_threads.h"

volatile const char * IDENT_PA_THREADS_C="$Id: pa_threads.C,v 1.8 2020/12/16 11:04:00 moko Exp $" IDENT_PA_THREADS_H;

Mutex global_mutex;

#ifdef _MSC_VER

uint pa_get_thread_id() {
	return GetCurrentThreadId();
}

Mutex::Mutex() :
	handle(CreateMutex(NULL, FALSE, 0)) {
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

#else

#include <pthread.h>
#ifdef HAVE_GETTID
#include <sys/syscall.h>
#endif

uint pa_get_thread_id() {
#ifdef HAVE_GETTID
	return syscall(__NR_gettid);
#else
	return getpid();
#endif
}

Mutex::Mutex() {
	pthread_mutex_init(&handle, NULL);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&handle);
}

void Mutex::acquire() {
	pthread_mutex_lock(&handle);
}

void Mutex::release() {
	pthread_mutex_unlock(&handle);
}

#endif
