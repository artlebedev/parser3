/** @file
	Parser: stack class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_stack.h,v 1.13 2002/02/08 08:30:14 paf Exp $
*/

#ifndef PA_STACK_H
#define PA_STACK_H

#include "pa_config_includes.h"
#include "pa_array.h"

/// simple stack based on Array
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
