/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

static const char * const IDENT_VJUNCTION_H="$Date: 2004/02/11 15:33:18 $";

// include

#include "pa_value.h"
#include "pa_vbool.h"
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
	override Value& as_expr_result(bool) { return *new VBool(as_bool()); }

	/// VJunction: method, root,self,rcontext, code
	override Junction* get_junction() { return fjunction; }

public: // usage

	VJunction(Junction* ajunction): fjunction(ajunction) {}

private:

	Junction* fjunction;
};


#endif
