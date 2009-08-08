/** @file
	Parser: @b console class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCONSOLE_H
#define PA_VCONSOLE_H

static const char * const IDENT_VCONSOLE_H="$Date: 2009/08/08 13:30:21 $";

// includes

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

// defines

#define CONSOLE_CLASS_NAME "console"
static const String console_class_name(CONSOLE_CLASS_NAME);

#define CONSOLE_LINE_NAME "line"

/// console class
class VConsole: public Value {
public: // Value
	
	const char* type() const { return CONSOLE_CLASS_NAME; }
	/// VConsole: 0
	VStateless_class *get_class() { return 0; }

	/// console: line, CLASS, CLASS_NAME
	Value* get_element(const String& aname) {
		// $line
		if(aname==CONSOLE_LINE_NAME) {
			char local_value[MAX_STRING];
			if(fgets(local_value, sizeof(local_value), stdin))
				return new VString(*new String(strdup(local_value), String::L_TAINTED));

			return 0; // EOF
		}

		// $CLASS
		if(aname==CLASS_NAME)
			return this;

		// $CLASS_NAME
		if(aname==CLASS_NAMETEXT)
			return new VString(console_class_name);

		throw Exception(PARSER_RUNTIME,
			&aname,
			"reading of invalid field");
	}

	/// console: $line
	override const VJunction* put_element(const String& aname, Value* avalue, bool /*areplace*/) { 
		// $line
		if(aname==CONSOLE_LINE_NAME) {
			fused=true;
			puts(avalue->as_string().cstr());
			fflush(stdout);

			return PUT_ELEMENT_REPLACED_ELEMENT;
		}

		throw Exception(PARSER_RUNTIME,
			&aname,
			"writing to invalid field");
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
