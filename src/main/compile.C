/*
  $Id: compile.C,v 1.12 2001/02/22 09:36:23 paf Exp $
*/

#include "pa_request.h"
#include "pa_string.h"
#include "pa_array.h"
#include "code.h"
#include "compile_tools.h"
#include "pa_exception.h"

#include <stdio.h>

extern int yydebug;
extern int yyparse (void *);

Array& Request::real_compile(COMPILE_PARAMS) {
	// prepare to parse
	struct parse_control pc;

	// input 
	pc.pool=&pool();
	pc.methods=new(pool()) Array(pool());
	if(!source)
		return *pc.methods;

	pc.source=source;
#ifndef NO_STRING_ORIGIN
	pc.file=file;
	pc.line=pc.col=0;
#endif
	// initialise state
	pc.pending_state=0;
	pc.string=new(pool()) String(pool());	
	pc.ls=LS_USER;
	pc.sp=0;
	
	// parse=compile! 
	///yydebug=1;
	if(yyparse(&pc)) { // error?
		if(pc.col==1) {
			// step back from last char.  -1 col means EOL
			pc.line--;
			pc.col=-1;
		}
		exception().raise(0,0,
			0,
			"%s [%s:%d:%d]", pc.error, file, 1+pc.line, pc.col);
	}

	// result
	return *pc.methods;
}
