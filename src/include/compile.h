/*
  $Id: compile.h,v 1.3 2001/02/21 11:10:02 paf Exp $
*/

#ifndef COMPILE_H
#define COMPILE_H

#include "pa_array.h"
#include "pa_request.h"

#ifndef NO_STRING_ORIGIN
#	define COMPILE_PARAMS Request& request, char *source, char *file
#	define COMPILE(pool, source, file) real_compile(pool, source, file)
#else
#	define COMPILE_PARAMS Request& request, String& name, char *source
#	define COMPILE(pool, source, file) real_compile(pool, source)
#endif

Array *real_compile(COMPILE_PARAMS);

#endif
