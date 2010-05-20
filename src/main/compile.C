/** @file
	Parser: compiler part of request class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_COMPILE_C="$Date: 2010/05/20 04:29:51 $";

#include "pa_request.h"
#include "compile_tools.h"

extern int yydebug;
extern int yyparse (void *);

ArrayClass& Request::compile(VStateless_class* aclass, 
					const char* source, const String* main_alias, 
					uint file_no,
					int line_no_offset) {
	// prepare to parse
	Parse_control pc(*this, aclass, source, main_alias, file_no, line_no_offset);

	// parse=compile! 
	//yydebug=1;
	if(yyparse(&pc)) { // error?
		pc.pos_prev_c();
		if(!pc.explicit_result)
			if(pc.pos.col==0) // expecting something after EOL means they've expected it BEFORE
				pc.pos_prev_c();

		throw Exception("parser.compile",
			0,
			"%s(%d:%d): %s",  file_list[file_no].cstr(), 1+pc.pos.line, 1+pc.pos.col,  pc.error);
	}

	// result
	return *pc.cclasses;
}
