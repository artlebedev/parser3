/*
  $Id: pa_vjunction.h,v 1.1 2001/02/24 11:20:32 paf Exp $
*/

#ifndef PA_VJUNCTION_H
#define PA_VJUNCTION_H

#include "pa_value.h"

class VJunction : public Value {
public: // VJunction

	// all: for error reporting after fail(), etc
	const char *type() const { return "Junction"; }

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
