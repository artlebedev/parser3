/** @file
	Parser: mutex & helpers decls.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_THREADS_H
#define PA_THREADS_H

static const char* IDENT_THREADS_H="$Date: 2003/07/24 11:31:21 $";

#include "pa_config_includes.h"
#include "pa_types.h"

/// thread ID type
typedef unsigned int pa_thread_t;

/// get caller thread ID
pa_thread_t pa_get_thread_id();

class AutoSYNCHRONIZED;

/// simple semaphore object
class Mutex {
	friend class AutoSYNCHRONIZED;
private:
	uint handle;
public:
	Mutex();
	~Mutex();
private: // for AutoSYNCHRONIZED
	void acquire();
	void release();
};

extern const bool parser_multithreaded;
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
