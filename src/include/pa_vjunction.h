/*
  $Id: pa_vjunction.h,v 1.2 2001/03/06 15:02:47 paf Exp $
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#include "pa_value.h"

class VJunction : public Value {
public: // VJunction

	// all: for error reporting after fail(), etc
	const char *type() const { return "junction"; }

	// junction: method, root,self,rcontext, code
	Junction *get_junction() { return &junction; }

public: // usage

	VJunction(Junction& ajunction) : Value(ajunction.pool()),
		junction(ajunction) {
	}

private:

	Junction& junction;
};


#endif
