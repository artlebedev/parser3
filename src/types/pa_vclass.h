/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

static const char* IDENT_VCLASS_H="$Date: 2002/08/09 14:18:39 $";

#include "pa_vstateless_class.h"
//#include "pa_vhash.h"
#include "pa_vjunction.h"
#include "pa_vobject.h"

/**	stores 
- base: VClass::base()
- static fields: VClass::ffields
*/
class VClass : public VStateless_class {
public: // Value
	
	const char *type() const { return name_cstr(); }

	Value *as(const char *atype) {
		if(Value *result=Value::as(atype))
			return result;
		else
			return fbase?fbase->as(atype):0;
	}

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

	/// @returns object of this class
	virtual Value *create_new_value(Pool& ) { 
		return NEW VObject(pool(), *this);
	}

protected: // VAliased

	void set_alias(VStateless_class *aclass_alias) {
		fclass_alias=aclass_alias;
	}
	VStateless_class *get_alias() {
		return fclass_alias;
	}

public: // usage

	VClass(Pool& apool) : VStateless_class(apool), 
		ffields(apool) {
	}

private:

	void set_field(const String& name, Value *value) {
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

private: // self

	Hash ffields;

private: // VAliased
	
	VStateless_class *fclass_alias;

};

#endif
