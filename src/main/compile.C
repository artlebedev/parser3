/*
  $Id: compile.C,v 1.17 2001/02/23 22:22:08 paf Exp $
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
	pc.methods=NEW Array(pool());
	if(!source)
		return *pc.methods;

	pc.source=source;
#ifndef NO_STRING_ORIGIN
	pc.file=file;
	pc.line=pc.col=0;
#endif
	// initialise state
	pc.pending_state=0;
	pc.string=NEW String(pool());	
	pc.ls=LS_USER;
	pc.sp=0;
	
	// parse=compile! 
	yydebug=1;
	if(yyparse(&pc)) { // error?
		if(pc.col==0) { // expecting something after EOL means they've expected it BEFORE
			// step back.  -1 col means EOL
			pc.line--;
			pc.col=-1;
		} 
		THROW(0,0,
			0,
			"%s(%d:%d): %s", file, 1+pc.line, pc.col, pc.error);
	}

	// result
	return *pc.methods;
}
