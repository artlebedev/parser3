/*
  $Id: pa_vjunction.h,v 1.4 2001/03/08 16:54:25 paf Exp $
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#include "pa_value.h"

class VJunction : public Value {
public: // VJunction

	// all: for error reporting after fail(), etc
	const char *type() const { return "fjunction"; }

	// clone
	Value *clone() { return NEW VJunction(fjunction); };

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
