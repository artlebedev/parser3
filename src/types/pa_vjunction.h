/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vjunction.h,v 1.8 2001/10/09 08:13:04 parser Exp $
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#include "pa_value.h"
#include "pa_vbool.h"

/// junction is method+self+context, implemented with Junction
class VJunction : public Value {
public: // VJunction

	const char *type() const { return "junction"; }

	/// VJunction: true
	bool as_bool() const { return true; }

	/// VJunction: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VJunction: method, root,self,rcontext, code
	Junction *get_junction() { return &fjunction; }

public: // usage

	VJunction(Junction& ajunction) : Value(ajunction.pool()),
		fjunction(ajunction) {
	}

private:

	Junction& fjunction;
};


#endif
