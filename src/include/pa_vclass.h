/*
  $Id: pa_vclass.h,v 1.18 2001/02/25 14:47:12 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_value.h"
#include "pa_vhash.h"
#include "pa_vstring.h"
#include "pa_vjunction.h"

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

class VClass : public Value {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "Class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& aname) {
		// $NAME=my name
		if(aname==NAME_NAME)
			return NEW VString(name());
		// $CLASS=my class=myself
		if(aname==CLASS_NAME)
			return this;
		// $BASE=my parent
		if(aname==BASE_NAME)
			return base();
		// $method=junction(this+method)
		if(Method *method=static_cast<Method *>(methods().get(aname))) {
			Junction& j=*NEW Junction(pool(), 
				*this,
				method,0,0,0,0);

			return NEW VJunction(j);
		}
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

	VClass(Pool& apool) : 
		Value(apool), 
		fields(apool),
		fmethods(apool),
		fbase(0) {
	}

	void add_method(const String& name, Method& method) {
		fmethods.put(name, &method);
	}
	Hash& methods() { return fmethods; }
	
	void set_base(VClass& abase) {
		// remember the guy
		fbase=&abase;
		
		// append base_method to my methods unless already there is one
		// 'virtual' here
		fmethods.merge_dont_replace(abase.methods());
	}
	VClass *base() { return fbase; }

	bool is_or_derived_from(VClass& vclass) {
		return 
			this==&vclass || 
			fbase && fbase->is_or_derived_from(vclass);
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
