/** @file
	Parser: Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_threads.C,v 1.3 2001/11/05 11:46:30 paf Exp $
*/

#include "pa_threads.h"

Mutex global_mutex;

#include "ap_config.h"
// defined in ap_config
#ifdef MULTITHREAD
#include "multithread.h"

const bool parser_multithreaded=true;

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

const bool parser_multithreaded=false;

Mutex::Mutex() {}
Mutex::~Mutex() {}
void Mutex::acquire() {}
void Mutex::release() {}

#endif
