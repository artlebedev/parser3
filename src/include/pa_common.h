/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_common.h,v 1.11 2001/03/13 19:35:04 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#include "pa_config.h"

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
char *format(Pool& pool, double value, char *fmt);

#ifndef max
inline int max(int a,int b) { return a>b?a:b; }
inline int min(int a,int b){ return a<b?a:b; }
#endif

size_t stdout_write(const char *buf);

#endif
