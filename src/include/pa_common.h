/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_common.h,v 1.8 2001/03/12 17:00:46 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#include <stdarg.h>
#include "pa_pool.h"

#ifdef WIN32
#define vsnprintf __vsnprintf 
#define snprintf __snprintf

int __vsnprintf(char *, size_t, const char *, va_list);
int __snprintf(char *, size_t, const char *, ...);
#endif

char *file_read(Pool& pool, const char *fname, bool fail_on_read_problem=true);
char *getrow(char **row_ref,char delim='\n');
//char *lsplit(char *,char);
char *lsplit(char **string_ref,char delim);
char *rsplit(char *string, char delim);


#endif
