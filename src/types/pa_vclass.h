/**	@file
	Parser: @b class parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

static const char* IDENT_VCLASS_H="$Date: 2002/08/13 13:02:40 $";

#include "pa_vstateless_class.h"
//#include "pa_vhash.h"
#include "pa_vjunction.h"
#include "pa_vobject.h"

/**	stores 
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

	/// VClass: (field)=STATIC value;(method)=method_ref with self=object_class
	Value *get_element(const String& aname, Value *aself) {
		// $method or other base element
		if(Value *result=VStateless_class::get_element(aname, aself))
			return result;

		// $field=static field
		if(Value *result=static_cast<Value *>(ffields.get(aname)))
			return result;

		return 0;
	}

	/// VClass: (field)=value - static values only
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace) {
		try {
			if(fbase && fbase->put_element(aname, avalue, true))
				return true; // replaced in base
		} catch(Exception) { /* ignore "can not store to stateless_class errors */ }

		if(replace)
			return ffields.put_replace(aname, avalue);
		else {
			ffields.put(aname, avalue);
			return false;
		}
	}

	/// @returns object of this class
	/*override*/ Value *create_new_value(Pool& ) { 
		return NEW VObject(pool(), *this);
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

};

#endif
