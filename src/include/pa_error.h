/*
  $Id: pa_error.h,v 1.3 2001/01/29 22:34:57 paf Exp $
*/

#ifndef PA_ERROR_H
#define PA_ERROR_H

#include <setjmp.h>

#include "pa_types.h"
#include "pa_string.h"

class Error {
public:

	// address for long jump to jump to 
	jmp_buf mark;

	Error() : valid(false) {}
	static void die(char *acomment);
	void prepare_begin();
	void raise(const String *aproblem_source, const char *comment_fmt, ...);

	const String *problem_source();
	const char *comment();

private:

	bool valid;

	const String *fproblem_source;
	char fcomment[MAX_STRING];
};

#define ERROR_BEGIN(error) error.prepare_begin(), (setjmp(error.mark)==0)

#endif
