/** @file
	Parser: @b env class decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VENV_H
#define PA_VENV_H

#define IDENT_PA_VENV_H "$Id: pa_venv.h,v 1.40 2015/10/26 01:22:01 moko Exp $"

// includes

#include "pa_sapi.h"
#include "pa_value.h"
#include "pa_string.h"

// defines

#define ENV_CLASS_NAME "env"
static const String env_class_name(ENV_CLASS_NAME);

/// env class
class VEnv: public Value {
	SAPI_Info& finfo;
public: // Value
	
	const char* type() const { return ENV_CLASS_NAME; }
	/// VEnv: 0
	VStateless_class *get_class() { return 0; }

	// env: field
	Value* get_element(const String& aname);

public: // usage

	VEnv(SAPI_Info& ainfo): 
		
		finfo(ainfo) {}

};

#endif
