/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

static const char* IDENT_VJUNCTION_H="$Date: 2002/09/20 09:26:33 $";

#include "pa_value.h"
#include "pa_vbool.h"

/// junction is method+self+context, implemented with Junction
class VJunction : public Value {
public: // VJunction

	const char *type() const { return "junction"; }

	/// VJunction: 0
	VStateless_class *get_class() { return 0; }

	/// VJunction: true
	bool as_bool() const { return true; }

	/// VJunction: true
	Value *as_expr_result(bool) { return NEW VBool(pool(), as_bool()); }

	/// VJunction: method, root,self,rcontext, code
	Junction *get_junction() { return &fjunction; }

	/// VJunction: wcontext transparent
	/*override*/ Value *get_element(const String& aname, Value *aself, bool looking_up);
	/// VJunction: wcontext transparent
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);
public: // usage

	VJunction(Junction& ajunction) : Value(ajunction.pool()),
		fjunction(ajunction) {
	}

private:

	Junction& fjunction;
};


#endif
