/*
  $Id: pa_stack.h,v 1.2 2001/02/22 13:33:24 paf Exp $
*/

#ifndef PA_STACK_H
#define PA_STACK_H

#include "pa_array.h"

class Stack : public Array {
public:

	Stack(Pool& apool) : Array(apool), ftop(0) {
	}

	void push(Item *item) {
		if(ftop<size()) // cell is already allocated?
			put(ftop, item); // use it
		else
			*this+=item; // append it
		ftop++;
	}
	Item *pop() {
		return get(--ftop);
	}

	Item *operator [] (int top_offset) {
		return get(ftop-top_offset-1);
	}

	int top() { return ftop; }

private:

	// deepest used index
	int ftop;

};

#endif
