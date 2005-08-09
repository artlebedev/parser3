/** @file
	Parser: commonly functions.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_COMMON_C="$Date: 2005/08/09 08:14:52 $"; 

#include "pa_config_includes.h"
#include "pa_os.h"

#	if defined(WIN32)
#		include <windows.h>
#	endif

#ifdef HAVE_FLOCK

int pa_lock_shared_blocking(int fd) { return flock(fd, LOCK_SH); }
int pa_lock_exclusive_blocking(int fd) { return flock(fd, LOCK_EX); }
int pa_lock_exclusive_nonblocking(int fd) { return flock(fd, LOCK_EX || LOCK_NB); }
int pa_unlock(int fd) { return flock(fd, LOCK_UN); }

#else
#ifdef HAVE__LOCKING

#define FLOCK(operation) lseek(fd, 0, SEEK_SET);  return _locking(fd, operation, 1)
int pa_lock_shared_blocking(int fd) { FLOCK(_LK_LOCK); }
int pa_lock_exclusive_blocking(int fd) { FLOCK(_LK_LOCK); }
int pa_lock_exclusive_nonblocking(int fd) { FLOCK(_LK_NBLCK); }
int pa_unlock(int fd) { FLOCK(_LK_UNLCK); }

#else
#ifdef HAVE_FCNTL

#define FLOCK(cmd, arg) struct flock ls={arg, SEEK_SET};  return fcntl(fd, cmd, &ls)
int pa_lock_shared_blocking(int fd) { FLOCK(F_SETLKW, F_RDLCK); }
int pa_lock_exclusive_blocking(int fd) { FLOCK(F_SETLKW, F_WRLCK); }
int pa_lock_exclusive_nonblocking(int fd) { FLOCK(F_SETLK, F_RDLCK); }
int pa_unlock(int fd) { FLOCK(F_SETLK, F_UNLCK); }

#else
#ifdef HAVE_LOCKF

#define FLOCK(fd, operation) lseek(fd, 0, SEEK_SET);  return lockf(fd, operation, 1)
int pa_lock_shared_blocking(int fd) { FLOCK(F_LOCK); } // on intel solaris man doesn't have doc on shared blocking
int pa_lock_exclusive_blocking(int fd) { FLOCK(F_LOCK); }
int pa_lock_exclusive_nonblocking(int fd) { FLOCK(F_TLOCK); }
int pa_unlock(int fd) { FLOCK(F_TLOCK); }

#else

#error unable to find file locking func

#endif
#endif
#endif
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
	return (select(0, NULL, NULL, NULL, &t)<0 ? errno : 0); 
#endif
}
