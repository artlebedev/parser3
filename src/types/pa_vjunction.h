/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

static const char * const IDENT_VJUNCTION_H="$Date: 2008/06/05 13:24:16 $";

// include

#include "pa_value.h"
#include "pa_junction.h"

/// junction is method+self+context, implemented with Junction
class VJunction: public Value {
public: // VJunction

	override const char* type() const { return "junction"; }

	/// VJunction: 0
	override VStateless_class *get_class() { return 0; }

	/// VJunction: false
	override bool is_defined() const { return false; }

	/// VJunction: false
	override bool as_bool() const { return is_defined(); }

	/// VJunction: false
	override Value& as_expr_result(bool);

	/// VJunction: method, root,self,rcontext, code
	override Junction* get_junction() { return &fjunction; }

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

private:

	Junction fjunction;
};


#endif
