/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_threads.C,v 1.7 2002/01/21 13:04:03 paf Exp $
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
