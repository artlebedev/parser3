/*
  $Id: pa_threads.h,v 1.1 2001/01/29 11:17:49 paf Exp $
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
	void lock();
	void unlock();
};

#else

class Mutex {
	Mutex() {}
	~Mutex() {}
	void lock() {}
	void unlock() {}
};

#endif

#endif
