/*
  $Id: compile.C,v 1.15 2001/02/22 14:14:07 paf Exp $
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
			pc.col=-2;
		}
		THROW(0,0,
			0,
			"%s [%s:%d:%d]", pc.error, file, 1+pc.line, 1+pc.col);
	}

	// result
	return *pc.methods;
}
