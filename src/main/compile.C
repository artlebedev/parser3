/** @file
	Parser: compiler part of request class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

volatile const char * IDENT_COMPILE_C="$Id: compile.C,v 1.91 2026/10/05 17:50:18 moko Exp $";

#include "pa_request.h"
#include "compile_tools.h"
#include "pa_vclass.h"

extern int yydebug;
extern int yyparse (Parse_control *);

ArrayClass& Request::compile(VStateless_class* aclass, const char* source, const String* main_alias, uint file_no, int line_no_offset, Code_cache* code) {
	// prepare to parse
	Parse_control pc(*this, aclass, source, main_alias, file_no, line_no_offset);
#ifdef CODE_CACHE
	if(code)
		code->file_no=file_no;
	pc.code_cache=code;
#endif

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

#ifdef CODE_CACHE
	if(code)
		Code_cache_manager::put(*code);
#endif

	// result
	return *pc.cclasses;
}

#ifdef CODE_CACHE

#define CLASS_ADD if(!pc.class_add()) \
	throw Exception("parser.compile", 0, "%s - class is already defined", pc.cclass->type());

/// replays the compilation using the cached methods, mirrors control_method and code_method in compile.y
ArrayClass& Code_cache::replay(Request& r, VStateless_class& aclass) {
	Parse_control pc(r, &aclass, "", 0 /*main_alias: only ^process has one*/, file_no, 0);
	const String* filespec=r.get_used_filespec(file_no);

	for(Array<Action>::Iterator i(actions); i; ) {
		Action action=i.next();
		switch(action.kind) {
			case CLASS:
				CLASS_ADD;
				pc.cclass_new=new VClass(action.name->cstr(), filespec);
				pc.append=false;
				break;
			case USE:
				CLASS_ADD;
				r.use_file(*action.name, filespec, action.origin);
				break;
			case BASE:
				CLASS_ADD;
				if(VStateless_class* base_class=r.get_class(*action.name))
					pc.cclass->get_class()->set_base(base_class);
				else
					throw Exception("parser.compile", 0, "'%s': undefined class in @BASE", action.name->cstr());
				break;
			case OPTION:
				if(*action.name==Symbols::LOCALS_SYMBOL)
					pc.set_all_vars_local();
				else if(*action.name==Symbols::PARTIAL_SYMBOL) {
					if(VStateless_class* existed=pc.get_existed_class(pc.cclass_new)) {
						if(!pc.reuse_existed_class(existed))
							throw Exception("parser.compile", 0, "can't append methods to '%s' - the class wasn't marked as partial", pc.cclass_new->type());
					} else
						pc.cclass_new->set_partial();
				} else if(*action.name==Symbols::STATIC_SYMBOL)
					pc.set_methods_call_type(Method::CT_STATIC);
				else if(*action.name==Symbols::DYNAMIC_SYMBOL)
					pc.set_methods_call_type(Method::CT_DYNAMIC);
				break;
			case METHOD:
				CLASS_ADD;
				pc.cclass->set_method(*action.name, action.method);
				break;
			case MAIN:
				pc.cclass->set_method(*action.name, action.method);
				break;
		}
	}

	return *pc.cclasses;
}
#endif
