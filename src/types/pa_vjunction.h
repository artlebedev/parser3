/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#define IDENT_PA_VJUNCTION_H "$Id: pa_vjunction.h,v 1.37 2020/12/15 17:10:41 moko Exp $"

// include

#include "pa_value.h"
#include "pa_junction.h"

#define JUNCTION_CLASS_NAME "junction"
static const String junction_class_name(JUNCTION_CLASS_NAME);

/// junction is method+self+context, implemented with Junction
class VJunction: public Value {
public: // VJunction

	override const char* type() const { return JUNCTION_CLASS_NAME; }

	/// VJunction: 0
	override VStateless_class *get_class() { return 0; }

	/// VJunction: false
	override bool is_defined() const { return false; }

	/// VJunction: false
	override bool as_bool() const { return false; }

	/// VJunction: false
	override Value& as_expr_result();

	/// VJunction: method, root,self,rcontext, code
	override Junction* get_junction() { return &fjunction; }

	// VJunction: CLASS, CLASS_NAME, name
	override Value* get_element(const String& aname);

public: // usage

	/// Code-Junction constructor
	VJunction(Value& aself,
		const Method* amethod,
		VMethodFrame* amethod_frame,
		Value* arcontext,
		WContext* awcontext,
		ArrayOperation* acode): fjunction(aself, amethod, amethod_frame, arcontext, awcontext, acode) {}

	/// Method-Junction or Getter-Junction constructor
	VJunction(Value& aself,
		const Method* amethod,
		bool ais_getter=false,
		String* aauto_name=0
	): fjunction(aself, amethod, ais_getter, aauto_name) {}

	const Junction& junction() const { return fjunction; }

	inline VJunction *get(Value& aself){
		return &(fjunction.self)==&aself?this:new VJunction(aself, fjunction.method);
	}

	void reattach(WContext *new_wcontext);

private:

	Junction fjunction;
};


#endif
