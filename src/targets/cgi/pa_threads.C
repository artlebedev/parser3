/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_threads.C,v 1.8 2002/02/08 07:27:50 paf Exp $
*/

#include "pa_threads.h"

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
