/*
  $Id: pa_stack.h,v 1.1 2001/02/21 17:36:30 paf Exp $
*/

#ifndef PA_STACK_H
#define PA_STACK_H

#include "pa_array.h"

class Stack : public Array {
public:

	Stack(Pool& apool) : Array(apool), top(0) {
	}

	void push(Item *item) {
		if(top<size()) // cell is already allocated?
			put(top, item); // use it
		else
			*this+=item; // append it
		top++;
	}
	Item *pop() {
		return get(--top);
	}

	Item *operator [] (int top_offset) {
		return get(top-top_offset-1);
	}

private:

	// deepest used index
	int top;

};

#endif
