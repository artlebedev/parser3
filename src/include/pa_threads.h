/*
  $Id: pa_threads.h,v 1.4 2001/01/29 12:16:07 paf Exp $
*/

#ifndef PA_THREADS_H
#define PA_THREADS_H

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#include "pa_types.h"


#ifdef MULTITHREAD

class Mutex {
	uint handle;
public:
	Mutex();
	~Mutex();
	void acquire();
	void release();
};

extern Mutex global_mutex;

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
}

#define SYNCHRONIZED(athread_safe) AutoSYNCHRONIZED autoSYNCHRONIZED(athread_safe)


#else // not MULTITHREAD-ed

#define SYNCHRONIZED(athread_safe) /* do nothing */

#endif


#endif
