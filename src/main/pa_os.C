/** @file
	Parser: commonly functions.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_config_includes.h"
#include "pa_os.h"

volatile const char * IDENT_PA_OS_C="$Id: pa_os.C,v 1.29 2026/10/27 23:05:00 moko Exp $" IDENT_PA_OS_H; 

unsigned int pa_lock_attempts=PA_LOCK_ATTEMPTS;

#ifdef _MSC_VER
#include <windows.h>

#define PA_SH_LOCK 2
#define PA_EX_LOCK 1
#define PA_ULOCK 0
#define FLOCK(operation) int status=pa_flock(fd, operation);

int pa_flock(int fd, int operation) {
    HANDLE hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) {
        return -1;
    }

    OVERLAPPED overlapped = {0};

    if (operation == PA_ULOCK) {
        return pa_set_errno(UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &overlapped)) ? 0 : -1;
    } else {
        DWORD flags = LOCKFILE_FAIL_IMMEDIATELY;
        if (operation == PA_EX_LOCK) {
            flags |= LOCKFILE_EXCLUSIVE_LOCK;
        }
        return pa_set_errno(LockFileEx(hFile, flags, 0, MAXDWORD, MAXDWORD, &overlapped)) ? 0 : -1;
    }
}

static int pa_errno() {
    switch(GetLastError()) {
        case ERROR_LOCK_VIOLATION: // real case: returning the same error as with _locking
            return EACCES;
        case ERROR_IO_PENDING:
            return EAGAIN;
        case ERROR_INVALID_HANDLE:
            return EBADF;
        case ERROR_NOT_LOCKED:
            return ENOLCK;
        case ERROR_FILE_NOT_FOUND: // the same errors as with opendir
        case ERROR_PATH_NOT_FOUND:
            return ENOENT;
        case ERROR_DIRECTORY:
            return ENOTDIR;
    }
    return EACCES;
}

int pa_set_errno(int status) {
    if (!status)
        errno = pa_errno();
    return status;
}

#else

#ifdef HAVE_FLOCK

#define PA_SH_LOCK LOCK_SH|LOCK_NB
#define PA_EX_LOCK LOCK_EX|LOCK_NB
#define PA_ULOCK LOCK_UN
#define FLOCK(operation) int status=flock(fd, operation);

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
	struct timespec t;
	t.tv_sec = secs;
	t.tv_nsec = usecs * 1000;
	// GC stops the threads with signals, the sleep goes on for the time left
	int result;
	while((result=nanosleep(&t, &t))<0 && errno==EINTR);
	return result<0 ? errno : 0;
#endif
}

const char *pa_ctime(time_t t) {
#ifdef _MSC_VER
	const char *result=ctime(&t); // the buffer is per thread
#else
	static THREAD_LOCAL char buffer[32];
	const char *result=ctime_r(&t, buffer);
#endif
	return result ? result : "";
}

#ifdef __linux__

#include <sys/syscall.h>

#ifndef sigev_notify_thread_id
#define sigev_notify_thread_id _sigev_un._tid
#endif

bool pa_timer_start(pa_timer *timer, int secs, int interval, void (*handler)(int)) {
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler=handler; // no SA_RESTART: the signal interrupts the blocking calls
	sigaction(SIGUSR1, &action, 0);

	// the signal is sent to the calling thread only
	struct sigevent event;
	memset(&event, 0, sizeof(event));
	event.sigev_notify=SIGEV_THREAD_ID;
	event.sigev_signo=SIGUSR1;
	event.sigev_notify_thread_id=(pid_t)syscall(SYS_gettid);
	if(timer_create(CLOCK_MONOTONIC, &event, timer))
		return false;

	struct itimerspec spec;
	memset(&spec, 0, sizeof(spec));
	spec.it_value.tv_sec=secs;
	spec.it_interval.tv_sec=interval;
	if(timer_settime(*timer, 0, &spec, 0)){
		timer_delete(*timer);
		return false;
	}
	return true;
}

void pa_timer_stop(pa_timer timer) {
	timer_delete(timer);
}

#else

bool pa_timer_start(pa_timer *, int, int, void (*)(int)) { return false; }
void pa_timer_stop(pa_timer) {}

#endif
