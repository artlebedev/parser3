/*
  $Id: pa_common.h,v 1.1 2001/01/29 20:46:21 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#include <stdarg.h>

#ifdef WIN32
#define vsnprintf __vsnprintf 
#define snprintf __snprintf

int __vsnprintf(char *, size_t, const char *, va_list);
int __snprintf(char *, size_t, const char *, ...);
#endif

#endif
