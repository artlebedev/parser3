/*
  $Id: compile.C,v 1.19 2001/02/25 09:14:02 paf Exp $
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

VClass& Request::real_compile(COMPILE_PARAMS) {
	// prepare to parse
	struct parse_control pc;

	// input 
	pc.pool=&pool();
	pc.request=this;
	VClass *vclass=NEW VClass(pool());
	if(default_name) // they forced some default_name?
		vclass->set_name(*default_name);
	pc.vclass=vclass;

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
	return *pc.vclass;
}
