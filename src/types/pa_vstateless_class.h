/** @file
	Parser: stateless class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstateless_class.h,v 1.10 2001/04/26 14:55:35 paf Exp $
*/

#ifndef PA_VSTATELESS_CLASS_H
#define PA_VSTATELESS_CLASS_H

#include "pa_valiased.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

class Temp_method;

/**
	object' class. 
	
	basically collection of methods [VStateless_class::fmethods, Method]

	@see VStateless_object, Temp_method
*/
class VStateless_class : public VAliased {
	friend Temp_method;
public: // Value
	
	/// all: for error reporting after fail(), etc
	const char *type() const { return "stateless_class"; }

	/// VStateless_class: this
	VStateless_class *get_class() { return this; }
	
	/// VStateless_class: +$method
	Value *get_element(const String& aname) {
		// $CLASS, $BASE
		if(Value *result=VAliased::get_element(aname))
			return result;
		// $method=junction(self+class+method)
		if(Junction *junction=get_junction(*this, aname))
			return NEW VJunction(*junction);
		
		return 0;
	}

public: // usage

	VStateless_class(Pool& apool, VStateless_class *abase=0) : VAliased(apool, *this), 
		fbase(abase),
		fmethods(apool) {
	}

	Method *get_method(const String& name) { 
		return static_cast<Method *>(fmethods.get(name)); 
	}

	void add_method(const String& name, Method& method) {
		put_method(name, &method);
	}
	void add_native_method(
		const char *cstr_name,
		Method::Call_type call_type,
		Native_code_ptr native_code,
		int min_numbered_params_count, int max_numbered_params_count);
	
	void set_base(VStateless_class& abase) {
		// remember the guy
		fbase=&abase;
	}
	VStateless_class *base() { return fbase; }

	bool is_or_derived_from(VStateless_class& vclass) {
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

	// just stubs, real onces defined below the hierarchy, in 
	virtual Value *get_field(const String& name) { return 0; }
	virtual bool replace_field(const String& name, Value *value) { return false; }

private: // Temp_method

	void put_method(const String& aname, Method *amethod) {
		fmethods.put(aname, amethod); 
	}
	
private:

	Hash fmethods;

protected:

	VStateless_class *fbase;

};

///	Auto-object used for temporarily substituting/removing class method
class Temp_method {
	VStateless_class& fclass;
	const String& fname;
	Method *saved_method;
public:
	Temp_method(VStateless_class& aclass, const String& aname, Method *amethod) : 
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
