/** @file
	Parser: mutex & helpers decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_threads.h,v 1.12 2001/04/03 14:39:02 paf Exp $
*/

#ifndef PA_THREADS_H
#define PA_THREADS_H

#include "pa_config_includes.h"

#include "pa_types.h"


#ifdef MULTITHREAD

/// simple semaphore object
class Mutex {
	uint handle;
public:
	Mutex();
	~Mutex();
	void acquire();
	void release();
};

extern Mutex global_mutex;

/** 
	Helper to ensure paired Mutex::acquire() and Mutex::release().

	Use it with SYNCHRONIZED macro
*/
class AutoSYNCHRONIZED {
	bool thread_safe;
public:
	AutoSYNCHRONIZED(bool athread_safe) : thread_safe(athread_safe) { 
		if(thread_safe)
			global_mutex.acquire(); 
	}
	~AutoSYNCHRONIZED() { 
		if(thread_safe)
			global_mutex.release(); 
	}
};

/** 
	put it to first line of a function to ensure thread safety.
	@verbatim
		void someclass::somefunc(...) { SYNCHRONIZED(thread_safe);
			...
		}
	@endverbatim

	considering @a thread_safe to be the object field to flag 
	whether safety is really needed in this particular object instance
*/
#define SYNCHRONIZED(athread_safe) AutoSYNCHRONIZED autoSYNCHRONIZED(athread_safe)

#define SYNCH_LOCK global_mutex.acqire()
#define SYNCH_UNLOCK global_mutex.release()

#else // not MULTITHREAD-ed

#define SYNCHRONIZED(athread_safe) /* do nothing */

#define SYNCH_LOCK 
#define SYNCH_UNLOCK 

#endif


#endif
