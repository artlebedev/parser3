/** @file
	Parser: stateless class decls.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_CLASS_H
#define PA_VSTATELESS_CLASS_H

static const char * const IDENT_VSTATELESS_CLASS_H="$Date: 2009/05/04 09:25:32 $";

// include

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_vjunction.h"
#include "pa_method.h"
#include "pa_vproperty.h"

// defines

#define CLASS_NAME "CLASS"
#define CLASS_NAMETEXT "CLASS_NAME"

// forwards

class VStateless_class;
typedef Array<VStateless_class*> ArrayClass;

class Temp_method;

/**
	object' class. stores
	- base: VClass::base()
	- methods: VStateless_class::fmethods

	@see Method, VStateless_object, Temp_method
*/
class VStateless_class: public Value {
	friend class Temp_method;

	const String* fname;
	mutable const char* fname_cstr;
	Hash<const String::Body, Method*> fmethods;

	bool flocked;
	bool fall_vars_local;
	bool fpartial;

protected:

	VStateless_class* fbase;
	Method* fscalar;
	Method* fdefault_getter;
	Method* fdefault_setter;

public: // Value
	
	const char* type() const { return "stateless_class"; }

	/// VStateless_class: this
	override VStateless_class *get_class() { return this; }
	/// VStateless_class: fbase
	override Value* base() { return fbase; }
	override Value* get_element(const String& aname, Value& aself, bool alooking_up);
	override Value& as_expr_result(bool /*return_string_as_is=false*/);

	override Value* get_default_getter(Value& aself, const String& aname);
	override void set_default_getter(Method* amethod);

	override Value* get_scalar(Value& aself);
	override void set_scalar(Method* amethod);

	override VJunction* get_default_setter(Value& aself, const String& aname);
	override void set_default_setter(Method* amethod);

public: // usage

	VStateless_class(
		const String* aname=0, 
		VStateless_class* abase=0):
		fname(aname),
		flocked(false),
		fbase(abase),
		fall_vars_local(false),
		fpartial(false),
		fscalar(0),
		fdefault_getter(0),
		fdefault_setter(0)
		{
	}

	void lock() { flocked=true; }

	const String& name() const { 
		if(!fname) {
			if(fbase)
				return fbase->name();

			throw Exception(PARSER_RUNTIME,
				0,
				"getting name of nameless class");
		}

		return *fname; 
	}
	const char* name_cstr() const{
		if(this) {
			if(!fname_cstr) // remembering last calculated, and can't reassign 'fname_cstr'!
				fname_cstr=name().cstr();
			return fname_cstr;
		} else
			return "<unknown>";
	}

	void set_name(const String& aname) {
		fname=&aname; 
	}

	Method* get_method(const String& aname) const { 
		return fmethods.get(aname);
	}

	bool is_vars_local(){
		return fall_vars_local;
	}

	void set_all_vars_local(){
		fall_vars_local=true;
	}

	bool is_partial(){
		return fpartial;
	}

	void set_partial(){
		fpartial=true;
	}

	/// virtual for VClass to override to pre-cache property accessors into fields
	virtual void add_method(const String& name, Method& method);
	
	void add_native_method(
		const char* cstr_name,
		Method::Call_type call_type,
		NativeCodePtr native_code,
		int min_numbered_params_count, 
		int max_numbered_params_count, 
		Method::Call_optimization call_optimization=Method::CO_WITHOUT_WCONTEXT);
	
	void set_base(VStateless_class* abase) {
		// remember the guy
		fbase=abase;
	}
	VStateless_class* base_class() { return fbase; }

	bool derived_from(VStateless_class& vclass) {
		return 
			fbase==&vclass || 
			fbase && fbase->derived_from(vclass);
	}

	//@{
	/// @name just stubs, real onces defined below the hierarchy
	virtual Value* get_field(const String&) { return 0; }
	virtual bool replace_field(const String&, Value*) { return false; }
	//@}

	/// @returns new value for current class, used in classes/ & VClass
	virtual Value* create_new_value(Pool&, HashStringValue& /*afields*/) { return 0; }

protected:

	void fill_properties(HashStringValue& acache);

private:

	void put_method(const String& aname, Method* amethod);	
};

///	Auto-object used for temporarily substituting/removing class method
class Temp_method {
	VStateless_class& fclass;
	const String& fname;
	Method* saved_method;
public:
	Temp_method(VStateless_class& aclass, const String& aname, Method* amethod) : 
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
