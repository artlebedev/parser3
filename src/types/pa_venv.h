/** @file
	Parser: env class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_venv.h,v 1.13 2001/03/23 10:27:34 paf Exp $
*/

#ifndef PA_VENV_H
#define PA_VENV_H

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

/// env class
class VEnv : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
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
