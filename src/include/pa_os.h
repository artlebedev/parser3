/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_OS_H
#define PA_OS_H

static const char * const IDENT_OS_H="$Date: 2004/02/11 15:33:14 $";

int pa_lock_shared_blocking(int fd);
int pa_lock_exclusive_blocking(int fd);
int pa_lock_exclusive_nonblocking(int fd);
int pa_unlock(int fd);

/// yields to OS for secs secs and usecs microseconds (1E-6)
int pa_sleep(unsigned long secs, unsigned long usecs);

#endif
