/*
  $Id: pa_error.h,v 1.2 2001/01/29 21:51:51 paf Exp $
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
	void raise(String *aproblem_source, char *comment_fmt, ...);

	String *problem_source();
	char *comment();

private:

	bool valid;

	String *fproblem_source;
	char fcomment[MAX_STRING];
};

#define ERROR_BEGIN(error) error.prepare_begin(), (setjmp(error.mark)==0)

#endif
