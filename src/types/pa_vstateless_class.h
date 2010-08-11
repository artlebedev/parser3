/** @file
	Parser: stateless class decls.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_CLASS_H
#define PA_VSTATELESS_CLASS_H

static const char * const IDENT_VSTATELESS_CLASS_H="$Date: 2010/08/11 16:21:52 $";

// include

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_vjunction.h"
#include "pa_method.h"

// defines

#define CLASS_NAME "CLASS"
#define CLASS_NAMETEXT "CLASS_NAME"
extern const String class_name, class_nametext;

// forwards

class VStateless_class;
typedef Array<VStateless_class*> ArrayClass;
typedef HashString<Property *> HashStringProperty;
typedef HashString<Method *> HashStringMethod;

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
	HashStringMethod fmethods;

	bool flocked;
	bool fall_vars_local;
	bool fpartial;
	Method::Call_type fcall_type;

protected:

	VStateless_class* fbase;
	/// all derived classes, recursively
	ArrayClass fderived;

	Method* fscalar;
	Method* fdefault_getter;
	Method* fdefault_setter;

public: // Value
	
	const char* type() const { return "stateless_class"; }

	/// VStateless_class: this
	override VStateless_class* get_class() { return this; }
	/// VStateless_class: fbase
	override VStateless_class* base() { return fbase; }

	override Value* get_element(const String& aname) { return get_element(*this, aname); }
	/// get_element with aself for VObject junctions
	virtual Value* get_element(Value& aself, const String& aname);

	override const VJunction* put_element(const String& aname, Value* avalue, bool areplace) {	return put_element(*this, aname, avalue, areplace); }
	/// put_element with aself for VObject junctions
	virtual const VJunction* put_element(Value& /*aself*/, const String& aname, Value* avalue, bool areplace) {	return Value::put_element(aname, avalue, areplace); }

	override Value& as_expr_result(bool /*return_string_as_is=false*/);

	Value* get_scalar(Value& aself);
	void set_scalar(Method* amethod);

	Value* get_default_getter(Value& aself, const String& aname);
	void set_default_getter(Method* amethod);
	bool has_default_getter();

	VJunction* get_default_setter(Value& aself, const String& aname);
	void set_default_setter(Method* amethod);
	bool has_default_setter();

	void add_derived(VStateless_class &aclass);

public: // usage

	VStateless_class(
		const String* aname=0, 
		VStateless_class* abase=0):
		fname(aname),
		flocked(false),
		fbase(0),
		fderived(0),
		fall_vars_local(false),
		fpartial(false),
		fscalar(0),
		fdefault_getter(0),
		fdefault_setter(0),
		fcall_type(Method::CT_ANY)
		{
			set_base(abase);
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
		if(!fname_cstr) // remembering last calculated, and can't reassign 'fname_cstr'!
			fname_cstr=name().cstr();
		return fname_cstr;
	}

	void set_name(const String& aname) {
		fname=&aname; 
		fname_cstr=0;
	}

	Method* get_method(const String& aname) const { 
		return fmethods.get(aname);
	}

	HashStringMethod get_methods(){
		return fmethods;
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

	Method::Call_type get_methods_call_type(){
		return fcall_type;
	}

	void set_methods_call_type(Method::Call_type call_type){
		if(fcall_type!=Method::CT_ANY)
			throw Exception(PARSER_RUNTIME,
						0,
						"You can specify call type option in a class only once"
					);
		fcall_type=call_type;
	}

	void add_native_method(
		const char* cstr_name,
		Method::Call_type call_type,
		NativeCodePtr native_code,
		int min_numbered_params_count, 
		int max_numbered_params_count, 
		Method::Call_optimization call_optimization=Method::CO_WITHOUT_WCONTEXT);

	void set_method(const String& aname, Method* amethod);

	/// overrided in VClass
	virtual void real_set_method(const String& aname, Method* amethod);
	virtual HashStringProperty* get_properties(){ return 0; };
	virtual void set_base(VStateless_class* abase);
	
	VStateless_class* base_class() { return fbase; }

	bool derived_from(VStateless_class& vclass){
		return fbase==&vclass || fbase && fbase->derived_from(vclass);
	}

	/// @returns new value for current class, used in classes/ & VClass
	virtual Value* create_new_value(Pool&) { return 0; }
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
		fclass.set_method(aname, amethod);
	}
	~Temp_method() { 
		fclass.set_method(fname, saved_method);
	}
};

#endif
