/*
  $Id: pa_exception.C,v 1.3 2001/01/30 13:43:43 paf Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "pa_exception.h"
#include "pa_common.h"

void Exception::raise(const String *atype, const String *acode,
					  const String *aproblem_source, 
					  const char *comment_fmt, ...) {
	ftype=atype;
	fcode=acode;
	fproblem_source=aproblem_source;

	if(comment_fmt) {
		va_list args;
		va_start(args, comment_fmt);
		vsnprintf(fcomment, MAX_STRING, comment_fmt, args);
		va_end(args);
	} else 
		fcomment[0]=0;

	longjmp(mark, 1);
}
