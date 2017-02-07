/** @file
	Parser: compiler part of request class.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

volatile const char * IDENT_COMPILE_C="$Id: compile.C,v 1.86 2017/02/07 22:00:40 moko Exp $";

#include "pa_request.h"
#include "compile_tools.h"

extern int yydebug;
extern int yyparse (Parse_control *);

ArrayClass& Request::compile(VStateless_class* aclass, const char* source, const String* main_alias, uint file_no, int line_no_offset) {
	// prepare to parse
	Parse_control pc(*this, aclass, source, main_alias, file_no, line_no_offset);

	// parse=compile! 
	//yydebug=1;
	if(yyparse(&pc)) { // error?
		pc.pos_prev_c();
		if(!pc.explicit_result)
			if(pc.pos.col==0) // expecting something after EOL means they've expected it BEFORE
				pc.pos_prev_c();

		exception_trace.push(Trace(0, Operation::Origin::create(file_no, pc.pos.line, pc.pos.col)));
		throw Exception("parser.compile", 0, "%s", pc.error);
	}

	// result
	return *pc.cclasses;
}
