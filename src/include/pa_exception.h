/*
  $Id: pa_exception.h,v 1.4 2001/02/21 14:59:41 paf Exp $
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#include <setjmp.h>

#include "pa_types.h"
#include "pa_string.h"

class Exception {
public:

	// address for long jump to jump to 
	// regretfully public:
	//   can't make local unless sure of inlining
	//   for to-die stack frame ruins it all
	jmp_buf mark;

	void raise(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		const char *comment_fmt, ...);

	const String *type() { return ftype; }
	const String *code() { return fcode; }
	const String *problem_source() { return fproblem_source; }
	const char *comment() { return fcomment[0]?fcomment:0; }

private:

	const String *ftype, *fcode, *fproblem_source;
	char fcomment[MAX_STRING];
};

#define EXCEPTION_TRY(e) (setjmp((e).mark)==0)
// usage:
//   if(EXCEPTION_TRY(e)) { ...; if(?) e.raise(?); ...; } else { catch-code }

#endif
