/** @file
	Parser: Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_threads.C,v 1.4 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_threads.h"

Mutex global_mutex;

#include "ap_config.h"
#include "multithread.h"
// defined in ap_config
#ifdef MULTITHREAD

Mutex::Mutex() : 
	handle(reinterpret_cast<uint>(ap_create_mutex(0))) {
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

#else

Mutex::Mutex() {}
Mutex::~Mutex() {}
void Mutex::acquire() {}
void Mutex::release() {}

#endif
