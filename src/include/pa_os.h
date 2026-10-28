/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_OS_H
#define PA_OS_H

#define IDENT_PA_OS_H "$Id: pa_os.h,v 1.19 2026/10/28 22:01:36 moko Exp $"

#define PA_LOCK_ATTEMPTS 20
#define PA_LOCK_WAIT_TIMEOUT_SECS 0
#define PA_LOCK_WAIT_TIMEOUT_USECS 500000

extern unsigned int pa_lock_attempts; // not thread local: never changes

// 'blocking' mean we will wait till other process release lock 
// but we'll make PA_LOCK_ATTEMPTS attempts with PA_LOCK_WAIT_TIMEOUT secs delaus between attempts 
// 'nonblocking' mean we will make only 1 attempt without waiting

int pa_lock_shared_blocking(int fd);
int pa_lock_exclusive_blocking(int fd);
int pa_lock_exclusive_nonblocking(int fd);
int pa_unlock(int fd);

/// yields to OS for secs secs and usecs microseconds (1E-6)
int pa_sleep(unsigned long secs, unsigned long usecs);

/// ctime in the buffer of the calling thread, valid till the next call; empty string on error
const char *pa_ctime(time_t t);

/// timer started with pa_timer_start
typedef void *pa_timer;

/// calls the signal handler in the calling thread after secs seconds and then every interval seconds if it is not 0
/// The handler interrupts the blocking calls (EINTR), it must be async-signal-safe, one handler is used by all timers
/// Linux only (needs gettid), returns false elsewhere or on error
bool pa_timer_start(pa_timer *timer, int secs, int interval, void (*handler)(int));
void pa_timer_stop(pa_timer timer);

#ifdef _MSC_VER
/// sets errno from GetLastError(), as CRT does, if the passed windows api call result is zero; returns it
int pa_set_errno(int status);
#endif

#endif
