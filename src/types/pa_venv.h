/** @file
	Parser: @b env class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VENV_H
#define PA_VENV_H

static const char* IDENT_VENV_H="$Date: 2002/10/31 15:01:55 $";

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

#define ENV_CLASS_NAME "env"

/// env class
class VEnv : public Value {
public: // Value
	
	const char *type() const { return "env"; }
	/// VEnv: 0
	VStateless_class *get_class() { return 0; }

	// VEnv: field
	Value *get_element(const String& aname, Value& /*aself*/, bool /*looking_up*/) {
		// getenv
		String& string=*NEW String(pool());
		const char *val=SAPI::get_env(pool(), aname.cstr());
		string.APPEND_TAINTED(val, 0, "environment", 0);
		return NEW VString(string);
	}

public: // usage

	VEnv(Pool& apool) : Value(apool) {
	}

};

#endif
