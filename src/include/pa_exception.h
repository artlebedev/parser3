/*
  $Id: pa_exception.h,v 1.1 2001/01/30 11:51:07 paf Exp $
*/

#ifndef PA_EXCEPTION_H
#define PA_EXCEPTION_H

#include <setjmp.h>

#include "pa_types.h"
#include "pa_string.h"

class Exception {
public:

	// address for long jump to jump to 
	jmp_buf mark;

	Exception();
	static void die(char *acomment);
	void raise(
		const String *atype, const String *acode,
		const String *aproblem_source, 
		const char *comment_fmt, ...);

	const String *type() { return ftype; }
	const String *code() { return fcode; }
	const String *problem_source() { return fproblem_source; }
	const char *comment() { return fcomment; }

private:

	const String *ftype, *fcode, *fproblem_source;
	char fcomment[MAX_STRING];
};

#define EXCEPTION_TRY(e) (setjmp(e.mark)==0)

#endif
