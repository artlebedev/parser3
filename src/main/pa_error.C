/*
  $Id: pa_error.C,v 1.2 2001/01/29 21:51:52 paf Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "pa_error.h"
#include "pa_common.h"

void Error::die(char *acomment) {
	// FIX: log(LOG_ERROR, acomment)
	fprintf(stderr, "die: %s\n", acomment);
	exit(1);
}

void Error::prepare_begin() {
	fproblem_source=0;
	fcomment[0]=0;
	valid=true;
}

void Error::raise(String *aproblem_source, char *comment_fmt, ...) {
	if(!valid)
		die("Error::raise(...) -- without begin()");

	fproblem_source=aproblem_source;
	
	va_list args;
    va_start(args, comment_fmt);
    vsnprintf(fcomment, MAX_STRING, comment_fmt, args);
    va_end(args);

	longjmp(mark, 1);
}

String *Error::problem_source() {
	if(!valid)
		die("Error::problem_source() -- without begin()");
	
	return fproblem_source;
}

char *Error::comment() {
	if(!valid)
		die("Error::comment() -- without begin()");
	
	return fcomment;
}
