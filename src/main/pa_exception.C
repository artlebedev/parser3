/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_exception.C,v 1.7 2001/03/19 16:06:14 paf Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "pa_common.h"
#include "pa_exception.h"

void Exception::_throw(const String *atype, const String *acode,
					  const String *aproblem_source, 
					  const char *comment_fmt, ...) const {
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
