/**	@file
	Parser: stateless class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)\
*/

#include "pa_vstateless_class.h"
#include "pa_vstring.h"
#include "pa_vbool.h"
#include "pa_symbols.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VSTATELESS_CLASS_C="$Id: pa_vstateless_class.C,v 1.63 2021/01/02 23:01:11 moko Exp $" IDENT_PA_VSTATELESS_CLASS_H IDENT_PA_METHOD_H;

bool VStateless_class::gall_vars_local=false;

override Value& VStateless_class::as_expr_result() {
	return VBool::get(as_bool());
}

/// @TODO why?! request must be different ptr from global [used in VStateless_class.set_method]
void VStateless_class::set_method(const String& aname, Method* amethod) {
	if(flocked)
		throw Exception(PARSER_RUNTIME, &aname, "can not add method to system class (maybe you have forgotten .CLASS in ^process[$caller.CLASS]{...}?)");

	if(fderived.count() && aname != auto_method_name) {
		Method *omethod=fmethods.get(aname);
		Array_iterator<VStateless_class *> i(fderived);
		while(i.has_next()) {
			VStateless_class *c=i.next();
			if(c->fmethods.get(aname)==omethod)
				c->real_set_method(aname, amethod);
		}
	}
	real_set_method(aname, amethod); 
}

void VStateless_class::real_set_method(const String& aname, Method* amethod) {
#ifdef SYMBOLS_CACHING
	// instance() as called from static constuctors
	Symbols::instance().add(aname);
#endif
	fmethods.put(aname, amethod);
	if(amethod)
		amethod->name=&aname;
}

void VStateless_class::add_native_method(
	const char* cstr_name,
	Method::Call_type call_type,
	NativeCodePtr native_code,
	int min_numbered_params_count, 
	int max_numbered_params_count,
	Method::Call_optimization
#ifdef OPTIMIZE_CALL
		call_optimization
#endif
	) {

	Method* method=new Method(
		call_type,
		min_numbered_params_count, max_numbered_params_count,
		0/*params_names*/, 0/*locals_names*/,
		0/*parser_code*/, native_code, false/*all_vars_local*/
#ifdef OPTIMIZE_RESULT
		, Method::RO_USE_WCONTEXT
#endif
#ifdef OPTIMIZE_CALL
		, call_optimization
#endif
		);

	set_method(*new String(cstr_name), method);
}

/// VStateless_class: $method
Value* VStateless_class::get_element(Value& aself, const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// $CLASS
	if(SYMBOLS_EQ(aname,CLASS_SYMBOL))
		return this;

	// $CLASS_NAME
	if(SYMBOLS_EQ(aname,CLASS_NAME_SYMBOL))
		return new VString(*new String(type()));
#endif

	// $method=junction(self+class+method)
	if(Method* method=get_method(aname))
		return method->get_vjunction(aself);

	return 0;
}

Value* VStateless_class::get_scalar(Value& aself){
	if(fscalar)
		return new VJunction(aself, fscalar, true /*getter*/);
	return 0;
}

void VStateless_class::set_scalar(Method* amethod){
	fscalar=amethod;
}

Value* VStateless_class::get_default_getter(Value& aself, const String& aname){
	if(fdefault_getter && aself.is_enabled_default_getter())
		return new VJunction(aself, fdefault_getter, true /*getter*/, (String*)&aname);
	return 0;
}

void VStateless_class::set_default_getter(Method* amethod){
	fdefault_getter=amethod;
}

bool VStateless_class::has_default_getter(){
	return fdefault_getter != NULL;
}

VJunction* VStateless_class::get_default_setter(Value& aself, const String& aname){
	if(fdefault_setter && aself.is_enabled_default_setter())
		return new VJunction(aself, fdefault_setter, false /*setter*/, (String*)&aname);
	return 0;
}

void VStateless_class::set_default_setter(Method* amethod){
	fdefault_setter=amethod;
}

bool VStateless_class::has_default_setter(){
	return fdefault_setter != NULL;
}

void VStateless_class::set_base(VStateless_class* abase){
	if(abase){
		abase->add_derived(*this);
		fbase=abase;

		bool no_auto = fmethods.get(auto_method_name) == NULL;
		// we assume there is no derivatives at this point
		fmethods.merge_dont_replace(abase->fmethods);
		// we don't want to inherit @auto (issue #75)
		if (no_auto) fmethods.remove(auto_method_name);

		if(fbase->fscalar && !fscalar)
			fscalar=fbase->fscalar;
		if(fbase->fdefault_getter && !fdefault_getter)
			fdefault_getter=fbase->fdefault_getter;
		if(fbase->fdefault_setter && !fdefault_setter)
			fdefault_setter=fbase->fdefault_setter;
	}
}

void VStateless_class::add_derived(VStateless_class &aclass){
	if(this == &aclass)
		throw Exception(PARSER_RUNTIME, 0, "circular class inheritance detected in class '%s'", type());
	fderived+=&aclass;
	if (fbase)
		fbase->add_derived(aclass);
}
