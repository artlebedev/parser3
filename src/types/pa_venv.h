/** @file
	Parser: @b env class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VENV_H
#define PA_VENV_H

static const char * const IDENT_VENV_H="$Date: 2004/02/11 15:33:17 $";

// includes

#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"
#include "pa_string.h"

// defines

#define ENV_CLASS_NAME "env"

/// env class
class VEnv: public Value {
	SAPI_Info& finfo;
public: // Value
	
	const char* type() const { return "env"; }
	/// VEnv: 0
	VStateless_class *get_class() { return 0; }

	// VEnv: field
	Value* get_element(const String& aname, Value& /*aself*/, bool /*looking_up*/) {
		// getenv
		String& result=*new String;
		if(const char* local_value=SAPI::get_env(finfo, aname.cstr()))
			result.append_help_length(strdup(local_value), 0, String::L_TAINTED);
		return new VString(result);
	}

public: // usage

	VEnv(SAPI_Info& ainfo): 
		
		finfo(ainfo) {}

};

#endif
