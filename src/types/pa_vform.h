/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vform.h,v 1.2 2001/03/13 20:02:11 paf Exp $
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

#include "pa_vstateless_class.h"
#include "_form.h"
#include "pa_common.h"

class VForm : public VStateless_class {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "form"; }

	// vform: this
	VStateless_class *get_class() { return this; }

	// form: CLASS,BASE,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(aname))
			return result;

		// $element
		return static_cast<Value *>(ffields.get(aname));
	}

public: // usage

	VForm(Pool& apool) : VStateless_class(apool, form_base_class),
		ffields(apool) {

		construct_fields();
	}

//	Hash& fields() { return ffields; }

private:

	void construct_fields();

private:

	Hash ffields;

};

#endif
