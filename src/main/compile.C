/** @file
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.C,v 1.32 2001/03/19 16:44:01 paf Exp $
*/

#include "code.h"
#include "pa_request.h"
#include "pa_string.h"
#include "pa_array.h"
#include "compile_tools.h"
#include "pa_exception.h"

#include <stdio.h>

extern int yydebug;
extern int yyparse (void *);

VStateless_class& Request::real_compile(COMPILE_PARAMS) {
	// prepare to parse
	struct parse_control pc;

	// input 
	pc.pool=&pool();
	pc.request=this;
	VStateless_class *cclass;
	if(aclass) // we were told the class to compile to?
		cclass=aclass; // yes, remember it [used in ^process]
	else if(name) { // we were told the name of compiled class?
		// yes. create it
		cclass=NEW VClass(pool());
		// defaulting base. may change with @BASE
		if(!base_class)
			base_class=&ROOT;
		cclass->set_base(*base_class);
		classes().put(*name, cclass);
		cclass->set_name(*name);
	} else
		cclass=&ROOT; // until changed with @CLASS would consider operators loading
	pc.cclass=cclass;

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
//	yydebug=1;
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
	return *pc.cclass;
}
