/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_stack.h,v 1.6 2001/03/11 08:16:33 paf Exp $
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

	int top_index() { return ftop-1; }
	Item *top_value() { return get(top_index()); }

private:

	// deepest used index
	int ftop;

};

#endif
