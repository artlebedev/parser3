/** @file
	Parser: stack class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STACK_H
#define PA_STACK_H

static const char* IDENT_STACK_H="$Date: 2003/07/24 11:31:21 $";

#include "pa_config_includes.h"
#include "pa_array.h"

/// simple stack based on Array
template<typename T> class Stack: public Array<T> {
public:

	Stack(): ftop(0) {}

	void push(T item) {
		if(ftop<count()) // cell is already allocated?
			put(ftop, item); // use it
		else
			*this+=item; // append it
		ftop++;
	}

	T pop() {
		return get(--ftop);
	}

	bool is_empty() { return ftop==0; }
	size_t top() { return ftop; }
	void top(size_t atop) { ftop=atop; }
	T upper_value() { 
		assert(!is_empty());
		return get(ftop-1); 
	}

	/// call this prior to collecting garbage [in unused part of stack there may be pointers(unused)]
	void wipe_unused() {
		if(!is_full())
			memset(&felements[fused], 0, (fallocated-fused)*sizeof(T));
	}

protected:

	// deepest used index+1
	size_t ftop;
};

#endif
