/** @file
	Parser: VCaller class decl. as wrapper for $caller access

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#ifndef PA_VCALLER_WRAPPER_H
#define PA_VCALLER_WRAPPER_H

#include "pa_value.h"
#include "pa_vmethod_frame.h"

#define IDENT_PA_VCALLER_WRAPPER_H "$Id: pa_vcaller_wrapper.h,v 1.3 2020/12/15 17:10:39 moko Exp $"

class VCallerWrapper: public Value {
	VMethodFrame &fcaller;

public: // Value

	override const char* type() const { return "caller"; }

	/// VCaller: transparent
	override VStateless_class* get_class() { return caller().get_class(); }

	/// VCaller: transparent
	override VStateless_class* base() { return caller().base(); }

	/// VCaller: empty string to disable $backup[$caller]
	override const String* get_string() { return &String::Empty; }

	/// VCaller: method or transparent
	override Value* get_element(const String& aname) {
		if(SYMBOLS_EQ(aname,METHOD_SYMBOL))
			return caller().method.get_vjunction(caller().self());

		return caller().get_element(aname);
	}

	/// VCaller: transparent
	override const VJunction* put_element(const String& aname, Value* avalue) { return caller().put_element(aname, avalue); }

public: // usage

	VCallerWrapper(VMethodFrame &acaller): fcaller(acaller) {}

	VMethodFrame &caller() { return fcaller; }

};

#endif
