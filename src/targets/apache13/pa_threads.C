/** @file
	Parser: Mutex realization class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_threads.C,v 1.6 2002/02/08 07:27:50 paf Exp $
*/

#include "pa_threads.h"

Mutex global_mutex;

#include "ap_config.h"
// defined in ap_config
#ifdef MULTITHREAD
#include "multithread.h"

const bool parser_multithreaded=true;

pa_thread_t pa_get_thread_id() {
	return static_cast<pa_thread_t>(
#ifdef WIN32
		GetThreadID()
#else
#	ifdef to-do-PTHREADS
			pthread_self()
#	else
#		ifdef to-do-SOLARIS_THREADS
			thr_self()
#		else
#			error there must be some get_thread_id function
#		endif
#	endif
#endif
		);
}

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

pa_thread_t pa_get_thread_id() { return 1; }

Mutex::Mutex() {}
Mutex::~Mutex() {}
void Mutex::acquire() {}
void Mutex::release() {}

#endif
