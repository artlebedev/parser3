/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vclass.h,v 1.4 2001/03/12 09:31:26 paf Exp $
*/

#ifndef PA_VCLASS_H
#define PA_VCLASS_H

#include "pa_valiased.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

class Temp_method;

class VClass : public VAliased {
	friend Temp_method;
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "class"; }

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Value *get_element(const String& aname);

	// object_class, operator_class: (field)=value - static values only
	void put_element(const String& name, Value *value);

	// object_class, object_instance: object_class
	VClass *get_class() { return this; }

public: // usage

	VClass(Pool& apool) : VAliased(apool, *this), 
		fbase(0),
		ffields(apool),
		fmethods(apool) {
	}

	void add_method(const String& name, Method& method) {
		fmethods.put(name, &method);
	}
	void add_native_method(
		const char *cstr_name,
		Native_code_ptr native_code,
		int min_numbered_params_count, int max_numbered_params_count);
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

	void set_field(const String& name, Value *value) {
		if(value) // used in ^process to temporarily remove @main
			value->set_name(name);
		if(fbase && fbase->replace_field(name, value))
			return;

		ffields.put(name, value);
	}

private:

	Value *get_field(const String& name) {
		Value *result=static_cast<Value *>(ffields.get(name));
		if(!result && fbase)
			result=fbase->get_field(name);
		return result;
	}
		
	bool replace_field(const String& name, Value *value) {
		return 
			(fbase && fbase->replace_field(name, value)) ||
			ffields.put_replace(name, value);
	}

private: // Temp_method

	// object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
	Method *get_method(const String& name) { 
		return static_cast<Method *>(fmethods.get(name)); 
	}

	// object_class, operator_class: (field)=value - static values only
	void put_method(const String& name, Method *method) { fmethods.put(name, method); }
	
private:

	VClass *fbase;
	Hash ffields;
	Hash fmethods;
};

class Temp_method {
	VClass& fclass;
	const String& fname;
	Method *saved_method;
public:
	Temp_method(VClass& aclass, const String& aname, Method *amethod) : 
		fclass(aclass),
		fname(aname),
		saved_method(aclass.get_method(aname)) {
		fclass.put_method(aname, amethod);
	}
	~Temp_method() { 
		fclass.put_method(fname, saved_method);
	}
};

#endif
