/** @file
	Parser: stack class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STACK_H
#define PA_STACK_H

static const char * const IDENT_STACK_H="$Date: 2005/03/16 14:37:51 $";

#include "pa_config_includes.h"
#include "pa_array.h"

/// simple stack based on Array
template<typename T> class Stack: public Array<T> {
public:

	Stack(): ftop(0) {}

	void push(T item) {
		if(ftop<this->count()) // cell is already allocated?
			put(ftop, item); // use it
		else
			*this+=item; // append it
		ftop++;
	}

	T pop() {
		return this->get(--ftop);
	}

	bool is_empty() { return ftop==0; }
	size_t top_index() { return ftop; }
	void set_top_index(size_t atop) { ftop=atop; }
	T top_value() { 
		assert(!is_empty());
		return this->get(ftop-1); 
	}

	/// call this prior to collecting garbage [in unused part of stack there may be pointers(unused)]
	void wipe_unused() {
		if(size_t above_top_size=this->fused-ftop)
			memset(&this->felements[ftop], 0, above_top_size*sizeof(T));
	}

protected:

	// deepest used index+1
	size_t ftop;
};

#endif
