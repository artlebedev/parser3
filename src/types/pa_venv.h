/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_venv.h,v 1.4 2001/03/13 14:02:51 paf Exp $
*/

#ifndef PA_VENV_H
#define PA_VENV_H

#include "pa_vstateless_class.h"
#include "pa_string.h"
#include "_env.h"

class VEnv : public VStateless_class {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "env"; }

	// env: CLASS,BASE,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(aname))
			return result;

		String& string=*NEW String(pool());
		char *file="environment";
		int line=3;
		char *value="<value>";// TODO: getenv
		string.APPEND_TAINTED(value, 0, file, line);
		return NEW VString(string);
	}

public: // usage

	VEnv(Pool& apool) : VStateless_class(apool) {
	}

private:
};

#endif
