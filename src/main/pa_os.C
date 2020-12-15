/** @file
	Parser: commonly functions.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"
#include "pa_os.h"

volatile const char * IDENT_PA_OS_C="$Id: pa_os.C,v 1.19 2020/12/15 17:10:36 moko Exp $" IDENT_PA_OS_H; 

unsigned int pa_lock_attempts=PA_LOCK_ATTEMPTS;

#ifdef _MSC_VER
#include <windows.h>
#endif

#ifdef HAVE_FLOCK

#define PA_SH_LOCK LOCK_SH|LOCK_NB
#define PA_EX_LOCK LOCK_EX|LOCK_NB
#define PA_ULOCK LOCK_UN
#define FLOCK(operation) int status=flock(fd, operation);

#else
#ifdef HAVE__LOCKING

#define PA_SH_LOCK _LK_NBLCK
#define PA_EX_LOCK _LK_NBLCK
#define PA_ULOCK _LK_UNLCK
#define FLOCK(operation) lseek(fd, 0, SEEK_SET); int status=_locking(fd, operation, 1);

#else
#ifdef HAVE_FCNTL

#define PA_SH_LOCK F_RDLCK
#define PA_EX_LOCK F_WRLCK
#define PA_ULOCK F_UNLCK
#define FLOCK(operation) struct flock ls={operation, SEEK_SET}; int status=fcntl(fd, F_SETLK, &ls); 

#else
#ifdef HAVE_LOCKF

#define PA_SH_LOCK F_TLOCK
#define PA_EX_LOCK F_TLOCK
#define PA_ULOCK F_ULOCK
#define FLOCK(operation) lseek(fd, 0, SEEK_SET); int status=lockf(fd, operation, 1);

#else

#error unable to find file locking func

#endif
#endif
#endif
#endif

int pa_lock(int fd, int attempts, int operation){
	while(true){
		FLOCK(operation);
		if(status==0)
			return 0;
		if(--attempts<=0)
			return errno;
		pa_sleep(PA_LOCK_WAIT_TIMEOUT_SECS, PA_LOCK_WAIT_TIMEOUT_USECS);
	}
};

int pa_lock_shared_blocking(int fd) {
	return pa_lock(fd, pa_lock_attempts, PA_SH_LOCK);
}

int pa_lock_exclusive_blocking(int fd) {
	return pa_lock(fd, pa_lock_attempts, PA_EX_LOCK);
}

int pa_lock_exclusive_nonblocking(int fd) {
	return pa_lock(fd, 1, PA_EX_LOCK);
}

int pa_unlock(int fd) {
	return pa_lock(fd, 1, PA_ULOCK);
}


int pa_sleep(unsigned long secs, unsigned long usecs) {
	if(usecs >= 1000000){
			secs += usecs/1000000;
			usecs = usecs%1000000;
	}

#ifdef _MSC_VER
	Sleep(secs * 1000 + usecs / 1000); 
	return 0;
#else
	struct timeval t;
	t.tv_sec = secs;
	t.tv_usec = usecs;
	return (select(0, NULL, NULL, NULL, &t)<0 ? errno : 0); 
#endif
}
