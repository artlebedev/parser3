/*
  $Id: pa_exception.h,v 1.5 2001/02/22 10:43:42 paf Exp $
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
	mutable jmp_buf mark;

	void _throw(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		const char *comment_fmt, ...) const;

	const String *type() const { return ftype; }
	const String *code() const { return fcode; }
	const String *problem_source() const { return fproblem_source; }
	const char *comment() const { return fcomment[0]?fcomment:0; }

private:

	mutable const String *ftype, *fcode, *fproblem_source;
	mutable char fcomment[MAX_STRING];
};

//#define EXCEPTION_TRY(e) (setjmp((e).mark)==0)
// usage:
//   if(EXCEPTION_TRY(e)) { ...; if(?) e.raise(?); ...; } else { catch-code }

#endif
