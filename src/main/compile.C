/*
  $Id: compile.C,v 1.2 2001/02/20 19:21:13 paf Exp $
*/

#include "pa_array.h"
#include "code.h"
#include "compile_tools.h"
#include "compile.h"

#include <stdio.h>

extern int yydebug;
extern int yyparse (void *);

Array *real_compile(COMPILE_PARAMS) {
	if(!source)
		return 0;

	yydebug=1;
	struct parse_control pc;
	/* input */
	pc.pool=pool;
	pc.source=source;
#ifndef NO_STRING_ORIGIN
	pc.file=file;
	pc.line=1;
#endif
	/* state to initial */
	pc.pending_state=0;
	pc.string=string_create(pool);
	pc.ls=LS_USER;
	pc.sp=0;
	/* parse! */
	int parse_error=yyparse(&pc);
	/* result */
	return parse_error?0:static_cast<Array *>(pc.result);
}
