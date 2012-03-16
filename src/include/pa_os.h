/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_OS_H
#define PA_OS_H

#define IDENT_PA_OS_H "$Id: pa_os.h,v 1.8 2012/03/16 09:24:09 moko Exp $"

#define PA_LOCK_ATTEMPTS 20
#define PA_LOCK_WAIT_TIMEOUT_SECS 0
#define PA_LOCK_WAIT_TIMEOUT_USECS 500000

// 'blocking' mean we will wait till other process release lock 
// but we'll make PA_LOCK_ATTEMPTS attempts with PA_LOCK_WAIT_TIMEOUT secs delaus between attempts 
// 'nonblocking' mean we will make only 1 attempt without waiting

int pa_lock_shared_blocking(int fd);
int pa_lock_exclusive_blocking(int fd);
int pa_lock_exclusive_nonblocking(int fd);
int pa_unlock(int fd);

/// yields to OS for secs secs and usecs microseconds (1E-6)
int pa_sleep(unsigned long secs, unsigned long usecs);

#endif
