/** @file
	Parser: @b console class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCONSOLE_H
#define PA_VCONSOLE_H

static const char * const IDENT_VCONSOLE_H="$Date: 2005/08/05 13:03:04 $";

// includes

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

// defines

#define CONSOLE_CLASS_NAME "console"

#define CONSOLE_LINE_NAME "line"

/// console class
class VConsole: public Value {
public: // Value
	
	const char* type() const { return "console"; }
	/// VConsole: 0
	VStateless_class *get_class() { return 0; }

	// VConsole: $line
	Value* get_element(const String& aname, Value& /*aself*/, bool /*looking_up*/) {
		// $line
		if(aname==CONSOLE_LINE_NAME) {
			char local_value[MAX_STRING];
			if(fgets(local_value, sizeof(local_value), stdin)) {
				return new VString(*new String(strdup(local_value), true));
			}

			return 0; // EOF
		}

		throw Exception("parser.runtime",
			&aname,
			"reading of invalid field");
	}

	/// VConsole: $line
	override bool put_element(const String& aname, Value* avalue, bool /*replace*/) { 
		// $line
		if(aname==CONSOLE_LINE_NAME) {
			const char* cstr=avalue->as_string().cstr();
			puts(cstr);
			fflush(stdout);

			return true;
		}

		throw Exception("parser.runtime",
			&aname,
			"writing to invalid field");
	}

public: // usage

	VConsole() {}

};

#endif
