/*
  $Id: compile.C,v 1.6 2001/02/21 07:31:41 paf Exp $
*/

#include "pa_string.h"
#include "pa_array.h"
#include "code.h"
#include "compile_tools.h"
#include "compile.h"
#include "pa_exception.h"

#include <stdio.h>

extern int yydebug;
extern int yyparse (void *);

Array *real_compile(COMPILE_PARAMS) {
	if(!source)
		return 0;

	Pool& pool=request.pool();

	yydebug=1;
	struct parse_control pc;
	/* input */
	pc.pool=&pool;
	pc.source=source;
#ifndef NO_STRING_ORIGIN
	pc.file=file;
	pc.line=pc.col=1;
#endif
	/* state to initial */
	pc.pending_state=0;
	pc.string=new(pool) String(pool);
	pc.ls=LS_USER;
	pc.sp=0;
	/* parse! */
	if(yyparse(&pc)) // parse, error?
		request.exception().raise(0,0,
			0,
			"%s @%s[%d:%d]", pc.error, file, pc.line, pc.col-1);
	
	/* result */
	return pc.result;
}
