/** @file
	Parser: @b env class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_venv.h,v 1.19 2002/02/08 07:27:52 paf Exp $
*/

#ifndef PA_VENV_H
#define PA_VENV_H

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

#define ENV_CLASS_NAME "env"

/// env class
class VEnv : public Value {
public: // Value
	
	const char *type() const { return "env"; }

	// VEnv: field
	Value *get_element(const String& aname) {
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
