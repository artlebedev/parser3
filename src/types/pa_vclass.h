/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vclass.h,v 1.12 2001/04/04 06:16:22 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_vstateless_class.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

class VClass : public VStateless_class {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& aname) {
		if(Value *result=VStateless_class::get_element(aname))
			return result;

		// $field=static field
		return get_field(aname);
	}

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value) {
		set_field(name, value);
	}

public: // usage

	VClass(Pool& apool) : VStateless_class(apool), 
		ffields(apool) {
	}

private:

	void set_field(const String& name, Value *value) {
		//if(value) // used in ^process to temporarily remove @main
			//value->set_name(name);
		if(fbase && fbase->replace_field(name, value))
			return;

		ffields.put(name, value);
	}

	Value *get_field(const String& name) {
		Value *result=static_cast<Value *>(ffields.get(name));
		if(!result && fbase)
			result=fbase->get_field(name);
		return result;
	}

protected:
		
	bool replace_field(const String& name, Value *value) {
		return 
			(fbase && fbase->replace_field(name, value)) ||
			ffields.put_replace(name, value);
	}

private:

	Hash ffields;
};

#endif
