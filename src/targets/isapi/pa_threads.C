/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_threads.C,v 1.7 2002/02/08 07:27:51 paf Exp $
*/

#include "pa_threads.h"

/// @test really?
const bool parser_multithreaded=true;

pa_thread_t pa_get_thread_id() {
	return GetThreadID();
}

Mutex global_mutex;

Mutex::Mutex() :
	handle(static_cast<uint>(CreateMutex(NULL, FALSE, 0))) {
}

Mutex::~Mutex() {
	CloseHandle(static_cast<HANDLE>(handle));
}

void Mutex::acquire() {
    WaitForSingleObject(static_cast<HANDLE>(handle), INFINITE);
}

void Mutex::release() {
    ReleaseMutex(static_cast<HANDLE>(handle));
}
