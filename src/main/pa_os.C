/** @file
	Parser: commonly functions.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_COMMON_C="$Date: 2003/11/06 08:50:26 $"; 

#include "pa_config_includes.h"
#include "pa_os.h"

#	if defined(WIN32)
#		include <windows.h>
#	endif

// locking constants
//#define PA_DEBUG_NO_LOCKING

#ifdef PA_DEBUG_NO_LOCKING

#ifdef HAVE_FLOCK

static int pa_lock_shared_blocking(int fd) { return flock(fd, LOCK_SH); }
static int pa_lock_exclusive_blocking(int fd) { return flock(fd, LOCK_EX); }
static int pa_lock_exclusive_nonblocking(int fd) { return flock(fd, LOCK_EX || LOCK_NB); }
static int pa_unlock(int fd) { return flock(fd, LOCK_UN); }

#else
#ifdef HAVE__LOCKING

#define FLOCK(operation) lseek(fd, 0, SEEK_SET);  return _locking(fd, operation, 1)
static int pa_lock_shared_blocking(int fd) { FLOCK(_LK_LOCK); }
static int pa_lock_exclusive_blocking(int fd) { FLOCK(_LK_LOCK); }
static int pa_lock_exclusive_nonblocking(int fd) { FLOCK(_LK_NBLCK); }
static int pa_unlock(int fd) { FLOCK(_LK_UNLCK); }

#else
#ifdef HAVE_FCNTL

#define FLOCK(cmd, arg) struct flock ls={arg, SEEK_SET};  return fcntl(fd, cmd, &ls)
static int pa_lock_shared_blocking(int fd) { FLOCK(F_SETLKW, F_RDLCK); }
static int pa_lock_exclusive_blocking(int fd) { FLOCK(F_SETLKW, F_WRLCK); }
static int pa_lock_exclusive_nonblocking(int fd) { FLOCK(F_SETLK, F_RDLCK); }
static int pa_unlock(int fd) { FLOCK(F_SETLK, F_UNLCK); }

#else
#ifdef HAVE_LOCKF

#define FLOCK(fd, operation) lseek(fd, 0, SEEK_SET);  return lockf(fd, operation, 1)
static int pa_lock_shared_blocking(int fd) { FLOCK(F_LOCK); } // on intel solaris man doesn't have doc on shared blocking
static int pa_lock_exclusive_blocking(int fd) { FLOCK(F_LOCK); }
static int pa_lock_exclusive_nonblocking(int fd) { FLOCK(F_TLOCK); }
static int pa_unlock(int fd) { FLOCK(F_TLOCK); }

#else

#error unable to find file locking func

#endif
#endif
#endif
#endif

#else
static int pa_lock_shared_blocking(int fd) { return 0; }
static int pa_lock_exclusive_blocking(int fd) { return 0; }
static int pa_lock_exclusive_nonblocking(int fd) { return 0; }
static int pa_unlock(int fd) { return 0; }

#endif

int pa_sleep(unsigned long secs, unsigned long usecs) {
	for (;  usecs >= 1000000; ++secs, usecs -= 1000000); 

#ifdef WIN32
	Sleep(secs * 1000 + usecs / 1000); 
	return 0;
#else
	struct timeval t;
	t.tv_sec = secs;
	t.tv_usec = usecs;
	return (select(0, NULL, NULL, NULL, &t) == -1 ? errno : 0); 
#endif
}
