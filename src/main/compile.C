/** @file
	Parser: compiler part of request class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_COMPILE_C="$Date: 2002/12/15 14:25:22 $";

#include "pa_opcode.h"
#include "pa_request.h"
#include "pa_string.h"
#include "pa_array.h"
#include "compile_tools.h"
#include "pa_exception.h"

extern int yydebug;
extern int yyparse (void *);

VStateless_class& Request::real_compile(COMPILE_PARAMS) {
	// prepare to parse
	struct parse_control pc;

	// input 
	pc.pool=&pool();
	pc.request=this;
	// we were told the class to compile to?
	pc.cclass=&aclass; // until changed with @CLASS would consider operators loading

	pc.source=source;
#ifndef NO_STRING_ORIGIN
	pc.file=file;
#endif
	pc.line=pc.col=0; // off the check, 'col' used in compile

	// initialise state
	pc.trim_bof=true;
	pc.pending_state=0;
	pc.string=NEW String(pool());	
	pc.ls=LS_USER;
	pc.ls_sp=0;
	pc.in_call_value=false;
	
	// parse=compile! 
	//yydebug=1;
	if(yyparse(&pc)) { // error?
		if(pc.col==0) { // expecting something after EOL means they've expected it BEFORE
			// step back.  -1 col means EOL
			pc.line--;
			pc.col=-1;
		} 
		throw Exception("parser.compile",
			0,
			"%s(%d:%d): %s",  file, 1+pc.line, pc.col,  pc.error);
	}

	// result
	return *pc.cclass;
}
