/** @file
	Parser: stateless class decls.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_CLASS_H
#define PA_VSTATELESS_CLASS_H

static const char* IDENT_VSTATELESS_CLASS_H="$Date: 2003/01/21 15:51:20 $";

#include "pa_hash.h"
#include "pa_vjunction.h"

// defines

#define CLASS_NAME "CLASS"

// forwards

class Temp_method;

/**
	object' class. stores
	- base: VClass::base()
	- methods: VStateless_class::fmethods

	@see Method, VStateless_object, Temp_method
*/
class VStateless_class: public Value {
	friend class Temp_method;
public: // Value
	
	const char *type() const { return "stateless_class"; }

	/// VStateless_class: this
	/*override*/ VStateless_class *get_class() { return this; }
	/// VStateless_class: fbase
	/*override*/ Value *base() { return fbase; }
	/*override*/ Value *get_element(const String& aname, Value& aself, bool looking_up);

public: // usage

	VStateless_class(Pool& apool, 
		const String *aname=0, 
		VStateless_class *abase=0) : Value(apool), 
		fname(aname),
        fbase(abase),
		fmethods(apool) {
	}

	const String& name() const { 
		if(!fname) {
			if(fbase)
				return fbase->name();

			throw Exception("parser.runtime",
				0,
				"getting name of nameless class");
		}

		return *fname; 
	}
	const char *name_cstr() const {
		return this?name().cstr():"<unknown>";
	}
	void set_name(const String& aname) {
		fname=&aname; 
	}

	Method *get_method(const String& name) const { 
		return static_cast<Method *>(fmethods.get(name)); 
	}

	void add_method(const String& name, Method& method);
	
	void add_native_method(
		const char *cstr_name,
		Method::Call_type call_type,
		Native_code_ptr native_code,
		int min_numbered_params_count, int max_numbered_params_count);
	
	VStateless_class *set_base(VStateless_class *abase) {
		VStateless_class *result=fbase;
		// remember the guy
		fbase=abase;
		return result;
	}
	VStateless_class *base_class() { return fbase; }

	bool derived_from(VStateless_class& vclass) {
		return 
			fbase==&vclass || 
			fbase && fbase->derived_from(vclass);
	}

	//@{
	/// @name just stubs, real onces defined below the hierarchy
	virtual Value *get_field(const String& ) { return 0; }
	virtual bool replace_field(const String& , Value *) { return false; }
	//@}

	/// @returns new value for current class, used in classes/ & VClass
	virtual Value *create_new_value(Pool& ) { return 0; }

private: // Temp_method

	void put_method(const String& aname, Method *amethod) {
		fmethods.put(aname, amethod); 
	}
	
private:

	const String *fname;
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
