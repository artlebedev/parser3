/** @file
	Parser: Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_threads.C,v 1.2 2001/05/17 12:51:05 parser Exp $
*/

#include "pa_threads.h"

Mutex global_mutex;

#include "ap_config.h"
#include "multithread.h"
// defined in ap_config
#ifdef MULTITHREAD

Mutex::Mutex() : 
	handle(reinterpret_cast<uint>(ap_create_mutex(0))) {
		if(!handle) {
			__asm {
				int 3;
			};
		}
			
}

Mutex::~Mutex() {
	ap_destroy_mutex(reinterpret_cast<mutex *>(handle));
}

void Mutex::acquire() {
	if(!handle)
			__asm {
				int 3;
			};
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
