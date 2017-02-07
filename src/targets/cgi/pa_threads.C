/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_threads.h"

volatile const char * IDENT_PA_THREADS_C="$Id: pa_threads.C,v 1.19 2017/02/07 22:00:45 moko Exp $" IDENT_PA_THREADS_H;

const bool parser_multithreaded=false;

pa_thread_t pa_get_thread_id() { return 1; }

Mutex global_mutex;

Mutex::Mutex() {
}

Mutex::~Mutex() {
}

void Mutex::acquire() {
}

void Mutex::release() {
}
