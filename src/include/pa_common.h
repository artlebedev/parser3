/*
  $Id: pa_common.h,v 1.2 2001/02/20 18:45:51 paf Exp $
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

char *file_read(Pool& pool, char *fname);

#endif
