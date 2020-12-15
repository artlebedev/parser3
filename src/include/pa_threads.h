/** @file
	Parser: mutex & helpers decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_THREADS_H
#define PA_THREADS_H

#define IDENT_PA_THREADS_H "$Id: pa_threads.h,v 1.37 2020/12/15 17:10:32 moko Exp $"

#include "pa_config_includes.h"
#include "pa_types.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

/// get caller thread ID
uint pa_get_thread_id();

class AutoSYNCHRONIZED;

/// simple semaphore object
class Mutex {
	friend class AutoSYNCHRONIZED;

#ifdef _MSC_VER
HANDLE handle;
#else
pthread_mutex_t handle;
#endif

public:
	Mutex();
	~Mutex();
private: // for AutoSYNCHRONIZED
	void acquire();
	void release();
};

extern Mutex global_mutex;

/** 
	Helper to ensure paired Mutex::acquire() and Mutex::release().
	Use it with SYNCHRONIZED macro
*/

class AutoSYNCHRONIZED {
public:
	AutoSYNCHRONIZED() { global_mutex.acquire(); }
	~AutoSYNCHRONIZED() { global_mutex.release(); }
};

/** 
	put it to first line of a function to ensure thread safety.
	@verbatim
		void someclass::somefunc(...) { SYNCHRONIZED;
			...
		}
	@endverbatim

	WARNING: don't use THROW or PTHROW with such thread safety mechanizm -
	longjmp would leave global_mutex acquired, which is wrong!
*/
#define SYNCHRONIZED AutoSYNCHRONIZED autoSYNCHRONIZED

#endif
