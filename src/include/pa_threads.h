/*
  $Id: pa_threads.h,v 1.2 2001/01/29 11:53:42 paf Exp $
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
public:
	AutoSYNCHRONIZED() { global_mutex.acquire(); }
	~AutoSYNCHRONIZED() { global_mutex.release(); }
}

#define SYNCHRONIZED AutoSYNCHRONIZED autoSYNCHRONIZED()


#else // not MULTITHREAD-ed

#define SYNCHRONIZED /* do nothing */

#endif


#endif
