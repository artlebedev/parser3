/**	@file
	Parser: @b junction class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vjunction.h,v 1.4 2001/04/26 15:01:51 paf Exp $
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#include "pa_value.h"

/// junction is method+self+context, implemented with Junction
class VJunction : public Value {
public: // VJunction

	// all: for error reporting after fail(), etc
	const char *type() const { return "junction"; }

	// fjunction: method, root,self,rcontext, code
	Junction *get_junction() { return &fjunction; }

public: // usage

	VJunction(Junction& ajunction) : Value(ajunction.pool()),
		fjunction(ajunction) {
	}

private:

	Junction& fjunction;
};


#endif
