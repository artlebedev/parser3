/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_venv.h,v 1.2 2001/03/13 13:43:32 paf Exp $
*/

#ifndef PA_VENV_H
#define PA_VENV_H

#include "pa_vstateless_object.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

class VEnv : public VClass {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "env"; }

	// env: CLASS,BASE,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(aname))
			return result;

		String& string=*new(pool) String(pool);
		char *file="environment";
		int line=3;
		char *value="<value>";// TODO: getenv
		string.APPEND_TAINTED(value, 0, file, line);
		return new(pool) VString(string));
	}

	// env: read-only
	void put_element(const String& name, Value *value) {
		bark("(%s) is read-only");
	}

public: // usage

	VEnv(Pool& apool) : VStateless_object(apool, *env_class) {
	}

private:
};

#endif
