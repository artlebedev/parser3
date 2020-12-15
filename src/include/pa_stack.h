/** @file
	Parser: stack class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STACK_H
#define PA_STACK_H

#define IDENT_PA_STACK_H "$Id: pa_stack.h,v 1.31 2020/12/15 17:10:32 moko Exp $"

#include "pa_array.h"

/// simple stack based on Array
template<typename T> class Stack: public Array<T> {
public:

	Stack(size_t initial=4) : Array<T>(initial){}

	inline void push(T item) {
		if(this->is_full())
			expand(this->fallocated); // free is not called, so expanding a lot to decrease memory waste
		this->felements[this->fused++]=item;
	}

	inline T pop() {
		return this->felements[--this->fused];
	}

	inline bool is_empty() { return this->fused==0; }
	inline size_t top_index() { return this->fused; }
	inline void set_top_index(size_t atop) { this->fused=atop; }
	inline T top_value() {
		assert(!is_empty());
		return this->felements[this->fused-1];
	}

	/// call this prior to collecting garbage [in unused part of stack there may be pointers(unused)]
	void wipe_unused() {
		if(size_t above_top_size=this->fallocated-this->fused)
			memset((void *)&this->felements[this->fused], 0, above_top_size*sizeof(T));
	}

protected:

	void expand(size_t delta) {
		size_t new_allocated=this->fallocated+delta;
		// we can't use realloc as MethodParams references allocated stack
		T* new_elements = (T *)pa_malloc(new_allocated*sizeof(T));
		memcpy(new_elements, this->felements, this->fallocated*sizeof(T));
		this->felements=new_elements;
		this->fallocated=new_allocated;
	}
};

#endif
