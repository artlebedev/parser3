/** @file
	Parser: Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_threads.C,v 1.1 2001/05/17 10:49:56 parser Exp $
*/

#include "pa_threads.h"

#ifdef MULTITHREAD

#include "ap_config.h"

Mutex global_mutex;

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

#endif
