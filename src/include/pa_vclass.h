/*
  $Id: pa_vclass.h,v 1.21 2001/03/07 13:54:46 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_valiased.h"
#include "pa_vhash.h"
#include "pa_vstring.h"
#include "pa_vjunction.h"

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

class VClass : public VAliased {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(String& aname) {
		// $NAME=my name
		if(aname==NAME_NAME)
			return NEW VString(class_alias->name());
		// $CLASS=my class=myself
		if(aname==CLASS_NAME)
			return class_alias;
		// $BASE=my parent
		if(aname==BASE_NAME)
			return class_alias->base();
		// $method=junction(self+class+method)
		if(Junction *junction=get_junction(*this, aname))
			return NEW VJunction(*junction);
		// $field=static field
		return get_field(aname);
	}

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value) {
		set_field(name, value);
	}

	// object_class, object_instance: object_class
	VClass *get_class() { return this; }

public: // usage

	VClass(Pool& apool) : VAliased(apool, *this), 
		fields(apool),
		fmethods(apool),
		fbase(0) {
	}

	void add_method(const String& name, Method& method) {
		fmethods.put(name, &method);
	}
//	Hash& methods() { return fmethods; }
	
	void set_base(VClass& abase) {
		// remember the guy
		fbase=&abase;
	}
	VClass *base() { return fbase; }

	bool is_or_derived_from(VClass& vclass) {
		return 
			this==&vclass || 
			fbase && fbase->is_or_derived_from(vclass);
	}

	Junction *get_junction(VAliased& self, const String& name) {
		if(Method *method=static_cast<Method *>(fmethods.get(name)))
			return NEW Junction(pool(), self, this, method, 0,0,0,0);
		if(fbase)
			return fbase->get_junction(self, name);
		return 0; 
	}

private:

	Value *get_field(const String& name) {
		Value *result=static_cast<Value *>(fields.get(name));
		if(!result && fbase)
			result=fbase->get_field(name);
		return result;
	}
		
	void set_field(const String& name, Value *value) {
		if(fbase && fbase->replace_field(name, value))
			return;

		fields.put(name, value);
	}
	bool replace_field(const String& name, Value *value) {
		return 
			(fbase && fbase->replace_field(name, value)) ||
			fields.put_replace(name, value);
	}
	
private:

	VClass *fbase;
	Hash fields;
	Hash fmethods;
};

#endif
