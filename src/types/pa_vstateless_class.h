/** @file
	Parser: stateless class decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VSTATELESS_CLASS_H
#define PA_VSTATELESS_CLASS_H

#define IDENT_PA_VSTATELESS_CLASS_H "$Id: pa_vstateless_class.h,v 1.94 2021/01/02 23:01:11 moko Exp $"

// include

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_vjunction.h"
#include "pa_method.h"

// forwards

class VStateless_class;
typedef Array<VStateless_class*> ArrayClass;
typedef HASH_STRING<Property *> HashStringProperty;
typedef HASH_STRING<Method *> HashStringMethod;

class Temp_method;

/**
	object' class. stores
	- base: VClass::base()
	- methods: VStateless_class::fmethods

	@see Method, VStateless_object, Temp_method
*/
class VStateless_class: public Value {
	friend class Temp_method;

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
	
	/// VStateless_class: this
	override VStateless_class* get_class() { return this; }
	/// VStateless_class: fbase
	override VStateless_class* base() { return fbase; }

	override Value* get_element(const String& aname) { return get_element(*this, aname); }
	/// get_element with aself for VObject junctions
	virtual Value* get_element(Value& aself, const String& aname);

	override const VJunction* put_element(const String& aname, Value* avalue) { return put_element(*this, aname, avalue); }
	/// put_element with aself for VObject junctions
	virtual const VJunction* put_element(Value& aself, const String& aname, Value* /*avalue*/) {
		aself.bark("element can not be stored to %s", &aname); 
		return 0;
	}

	override Value& as_expr_result();

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

	static bool gall_vars_local; // @conf[] $MAIN:LOCALS

	VStateless_class(VStateless_class* amethoded_donor=0):
		flocked(false),
		fall_vars_local(gall_vars_local),
		fpartial(false),
		fcall_type(Method::CT_ANY),
		fbase(0),
		fderived(0),
		fscalar(0),
		fdefault_getter(0),
		fdefault_setter(0)
	{
		if(amethoded_donor)
			fmethods.merge_dont_replace(amethoded_donor->fmethods);
	}

	void lock() { flocked=true; }

	Method* get_method(const String::Body &aname) const {
		return fmethods.get(aname);
	}

	HashStringMethod& get_methods(){
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
			throw Exception(PARSER_RUNTIME, 0, "You can specify call type option in a class only once");
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
		return fbase==&vclass || ( fbase && fbase->derived_from(vclass) );
	}

	/// @returns new value for current class, used in classes/ & VClass
	virtual Value* create_new_value(Pool&) { return 0; }

	const Method* get_element_method(const String &aname) {  // not get_method() to allow $aname[$method]
		if(Value* value=get_element(aname))
			if(Junction* junction=value->get_junction())
				return junction->method;
		return 0;
	}

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


class VBaseClassWrapper: public Value {

	VStateless_class& fclass;
	Value& fself;

public: // Value
	
	override const char* type() const { return fclass.type(); }
	override VStateless_class* get_class() { return &fclass; }
	override VStateless_class* base() { return fclass.base(); }

	override Value* get_element(const String& aname) { return fclass.get_element(fself, aname); }
	override const VJunction* put_element(const String& aname, Value* avalue) { return fclass.put_element(fself, aname, avalue); }
	override Value& as_expr_result(){ return fclass.as_expr_result(); }

public: // usage

	VBaseClassWrapper(VStateless_class& aclass, Value& aself): fclass(aclass), fself(aself) {};
};

#endif
