/*
  $Id: pa_vjunction.h,v 1.7 2001/03/10 15:17:44 paf Exp $
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#include "pa_value.h"

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
