/*
  $Id: pa_error.h,v 1.1 2001/01/29 20:46:21 paf Exp $
*/

#ifndef PA_ERROR_H
#define PA_ERROR_H

#include <setjmp.h>

#include "pa_types.h"
#include "pa_string.h"

class Error {
public:

	Error() : valid(false) {}
	static void die(char *acomment);
	bool begin();
	void raise(String *aproblem_source, char *comment_fmt, ...);

	String *problem_source();
	char *comment();

private:

	bool valid;

	// address for long jump to jump to 
	jmp_buf mark;

	String *fproblem_source;
	char fcomment[MAX_STRING];
};

#endif
