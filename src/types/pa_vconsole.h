/** @file
	Parser: @b console class decl.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VCONSOLE_H
#define PA_VCONSOLE_H

#define IDENT_PA_VCONSOLE_H "$Id: pa_vconsole.h,v 1.31 2025/05/26 01:56:54 moko Exp $"

// includes

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_vstateless_class.h"
#include "pa_string.h"

// defines

#define CONSOLE_LINE_NAME "line"

/// console class
class VConsole: public VStateless_class {
public: // Value
	
	const char* type() const { return "console"; }

	/// console: line
	Value* get_element(const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
		// CLASS, CLASS_NAME
		if(Value* result=VStateless_class::get_element(aname))
			return result;
#endif

		// $line
		if(aname==CONSOLE_LINE_NAME) {
			char local_value[MAX_STRING];
			if(fgets(local_value, sizeof(local_value), stdin))
				return new VString(pa_strdup(local_value));

			return 0; // EOF
		}

		return bark("%s field not found", &aname);
	}

	/// console: $line
	override const VJunction* put_element(const String& aname, Value* avalue) { 
		// $line
		if(aname==CONSOLE_LINE_NAME) {
			fused=true;
			puts(avalue->as_string().cstr());
			fflush(stdout);

			return 0;
		}

		throw Exception(PARSER_RUNTIME, &aname, "writing to invalid field");
	}

	bool was_used(){
		return fused;
	}

public: // usage

	VConsole() {
		fused=false;
	}

private:
	bool fused;
};

#endif
