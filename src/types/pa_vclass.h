/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vclass.h,v 1.20 2001/10/22 08:27:44 parser Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_vstateless_class.h"
//#include "pa_vhash.h"
#include "pa_vjunction.h"

/**	stores 
	- base: VClass::base()
	- static fields: VClass::ffields
*/
class VClass : public VStateless_class {
public: // Value
	
	const char *type() const { return "class"; }

	/// VClass: true
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }
	/// VClass: true
	bool as_bool() const { return true; }

	// VClass: (field)=STATIC value;(method)=method_ref with self=object_class
	Value *get_element(const String& aname) {
		// $field=static field
		if(Value *result=get_field(aname))
			return result;
		
		// $CLASS,$method
		return VStateless_class::get_element(aname);
	}

	// VClass: (field)=value - static values only
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
