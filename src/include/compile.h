/*
  $Id: compile.h,v 1.1 2001/02/20 18:45:51 paf Exp $
*/

#ifndef COMPILE_H
#define COMPILE_H

#include "pa_array.h"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS Pool *pool, char *source, char *file
#	define COMPILE(pool, source, file) real_compile(pool, source, file)
#else
#	define COMPILE_PARAMS Pool *pool, char *source
#	define COMPILE(pool, source, file) real_compile(pool, source)
#endif

Array *real_compile(COMPILE_PARAMS);

#endif
