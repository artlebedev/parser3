/** @file
	Parser: stack class decl.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_STACK_H
#define PA_STACK_H

#define IDENT_PA_STACK_H "$Id: pa_stack.h,v 1.33 2024/09/16 23:22:52 moko Exp $"

#include "pa_array.h"

/// simple stack based on Array
template<typename T> class Stack: public Array<T> {
public:

	Stack(size_t initial=4) : Array<T>(initial){}

	inline void push(T item) {
		if(this->is_full())
			expand(this->fallocated); // free is not called, so expanding a lot to decrease memory waste
		this->felements[this->fsize++]=item;
	}

	inline T pop() {
		return this->felements[--this->fsize];
	}

	inline bool is_empty() { return this->fsize==0; }
	inline size_t top_index() { return this->fsize; }
	inline void set_top_index(size_t atop) { this->fsize=atop; }
	inline T top_value() {
		assert(!is_empty());
		return this->felements[this->fsize-1];
	}

	/// call this prior to collecting garbage [in unused part of stack there may be pointers(unused)]
	void wipe_unused() {
		if(size_t above_top_size=this->fallocated-this->fsize)
			memset((void *)&this->felements[this->fsize], 0, above_top_size*sizeof(T));
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
