/** @file
	Parser: @b env class decl.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VENV_H
#define PA_VENV_H

#define IDENT_PA_VENV_H "$Id: pa_venv.h,v 1.43 2020/10/26 23:15:52 moko Exp $"

// includes

#include "pa_sapi.h"
#include "pa_vstateless_class.h"

/// env class
class VEnv: public VStateless_class {
	SAPI_Info& finfo;
public: // Value
	
	const char* type() const { return "env"; }

	// env: field
	Value* get_element(const String& aname);

	/// request: (key)=value
	override const VJunction* put_element(const String& name, Value* value);

public: // usage

	VEnv(SAPI_Info& ainfo): 
		
		finfo(ainfo) {}

};

#endif
