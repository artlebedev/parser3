/** @file
	Parser: compiler part of request class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: compile.C,v 1.55 2002/02/07 11:16:27 paf Exp $
*/

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
	VStateless_class *cclass;
	if(aclass) // we were told the class to compile to?
		cclass=aclass; // yes, remember it [used in ^process]
	else if(name) { // we were told the name of compiled class?
		// yes. create it
		cclass=NEW VClass(pool());
		// defaulting base. may change with @BASE
		// dont use ?: for there would be getting & of temp object
		if(base_class)
			cclass->set_base(*base_class);
		classes().put(*name, cclass);
		cclass->set_name(*name);
	} else
		cclass=&OP; // until changed with @CLASS would consider operators loading
	pc.cclass=cclass;

	pc.source=source;
#ifndef NO_STRING_ORIGIN
	pc.file=file;
	pc.line=pc.col=0;
#endif
	// initialise state
	pc.trim_bof=true;
	pc.pending_state=0;
	pc.string=NEW String(pool());	
	pc.ls=LS_USER;
	pc.ls_sp=0;
	pc.object_constructor_allowed=false;
	
	// parse=compile! 
	//yydebug=1;
	if(yyparse(&pc)) { // error?
		if(pc.col==0) { // expecting something after EOL means they've expected it BEFORE
			// step back.  -1 col means EOL
			pc.line--;
			pc.col=-1;
		} 
		throw Exception(0,0,
			0,
#ifndef NO_STRING_ORIGIN
			"%s(%d:%d): "
#endif
			"%s", 
#ifndef NO_STRING_ORIGIN
			file, 1+pc.line, pc.col, 
#endif
			pc.error);
	}

	// result
	return *pc.cclass;
}
