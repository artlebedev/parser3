/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vstateless_class.h,v 1.2 2001/03/13 13:43:33 paf Exp $
*/

#ifndef PA_VSTATELESS_CLASS_H
#define PA_VSTATELESS_CLASS_H

#include "pa_valiased.h"
#include "pa_vhash.h"
#include "pa_vjunction.h"

#define CLASS_NAME "class"
#define BASE_NAME "base"

class Temp_method;

class VStateless_class : public VAliased {
	friend Temp_method;
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "stateless_class"; }

	// stateless_class: $NAME,$CLASS,$BASE,$method
	Value *get_element(const String& aname);

	// stateless_class: object_class
	VStateless_class *get_class() { return this; }

public: // usage

	VStateless_class(Pool& apool) : VAliased(apool, *this), 
		fbase(0),
		read_only(false),
		fmethods(apool) {
	}

	Method *get_method(const String& name) { 
		return static_cast<Method *>(fmethods.get(name)); 
	}

	// make class read-only
	//	this blocks put_method  // which could be done with ^process
	void freeze() { read_only=true; }

	void add_method(const String& name, Method& method) {
		put_method(name, &method);
	}
	void add_native_method(
		const char *cstr_name,
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

	void put_method(const String& aname, Method *amethod);
	
private:

	Hash fmethods;

protected:

	VStateless_class *fbase;
	bool read_only;

};

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
