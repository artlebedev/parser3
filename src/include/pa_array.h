/** @file
	Parser: Array & Array_iterator classes decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_ARRAY_H
#define PA_ARRAY_H

static const char * const IDENT_ARRAY_Y="$Date: 2006/11/03 17:32:30 $";

// includes

#include "pa_memory.h"
#include "pa_exception.h"

// forwards

template<typename T> class Array_iterator;

// defines

#define ARRAY_OPTION_LIMIT_ALL ((size_t)-1)

/// Simple Array
template<typename T> class Array: public PA_Object {

	friend class Array_iterator<T>;

protected:

	/// elements[growing size] here
	T *felements;

	// allocated size
	size_t fallocated;

	// array size
	size_t fused;

public:
	struct Action_options {
		size_t offset;
		size_t limit; //< ARRAY_OPTION_LIMIT_ALL means 'all'. zero limit means 'nothing'
		bool reverse;
		bool defined;
		
		Action_options(
			size_t aoffset=0, 
			size_t alimit=ARRAY_OPTION_LIMIT_ALL, 
			bool areverse=false): 
			offset(aoffset), limit(alimit), reverse(areverse), 
			defined(false) {}

		bool adjust(size_t count) {
			if(!count || !limit)
				return false;
			size_t row=offset;
			if(row>=count)
				return false;
			// max(limit)
			size_t m=reverse?
				offset+1
				:count-offset;
			if(!m)
				return false;
			// fix limit
			if(limit==ARRAY_OPTION_LIMIT_ALL || limit>m)
				limit=m;

			return true;
		}

		
	};

	typedef T element_type;

	Array(size_t initial=3):
		fallocated(initial>3?initial:3),
		fused(0)
	{
		felements=static_cast<T*>(malloc(fallocated*sizeof(T)));
	}

	/// how many items are in Array
	size_t count() const { return fused; }
	/// append to array
	Array& operator += (T src) {
		if(is_full())
			expand(+2);

		felements[fused++]=src;

		return *this;
	}

	/// append other Array portion to this one. starting from offset
	Array& append(const Array& src, 
		size_t offset=0, 
		size_t limit=ARRAY_OPTION_LIMIT_ALL, //< negative limit means 'all'. zero limit means 'nothing'
		bool reverse=false) {

		size_t src_count=src.count();
		// skip tivials
		if(!src_count || !limit || offset>=src_count)
			return *this;
		// max(limit)
		size_t m=reverse?
			1+offset
			:src_count-offset;
		if(!m)
			return *this;
		// fix limit
		if(limit==ARRAY_OPTION_LIMIT_ALL || limit>m)
			limit=m;

		ssize_t delta=reverse?
			(ssize_t)limit
			:limit-(fallocated-fused);
		if(delta>0)
			expand(delta);

		T* from=&src.felements[offset];
		T* to=&felements[fused];
		if(reverse) { // reverse
			for(T* from_end=from-limit; from>from_end; --from)
				*to++=*from;

		} else { // forward
			for(T* from_end=from+limit; from<from_end; from++)
				*to++=*from;
		}
		
		fused+=limit;
		return *this;
	}

	/// get index-element
	T get(size_t index) const {
		assert(index<count());
		return felements[index];
	}

	/// ref version of get
	T& get_ref(size_t index) const {
		assert(index<count());
		return felements[index];
	}

	/// put index-element
	void put(size_t index, T element) {
		assert(index<count());
		felements[index]=element;
	}

	T operator [](size_t index) const { return get(index); }

	/// iterate over all elements
	template<typename I> void for_each(void (*callback)(T, I), I info) const {
		T *last=felements+fused;
		for(T *current=felements; current<last; current++)
			callback(*current, info);
	}

	/// iterate over all elements
	template<typename I> void for_each(bool (*callback)(T, I), I info) const {
		T *last=felements+fused;
		for(T *current=felements; current<last; current++)
			if(callback(*current, info))
				return;
	}

	/// iterate over all elements
	template<typename I> void for_each_ref(void (*callback)(T&, I), I info) {
		T *last=felements+fused;
		for(T *current=felements; current<last; current++)
			callback(*current, info);
	}

	/// iterate over all elements until condition becomes true, return that element
	template<typename I> T first_that(bool (*callback)(T, I), I info) const {
		T *last=felements+fused;
		for(T *current=felements; current<last; current++)
			if(callback(*current, info))
				return *current;

		return T(0);
	}

protected:

	bool is_full() {
		return fused == fallocated;
	}
	void expand(size_t delta) {
		size_t new_allocated=fallocated+delta;
		felements = (T *)realloc(felements, new_allocated*sizeof(T));
		memset(&felements[fallocated], 0, delta*sizeof(T));
		fallocated=new_allocated;
	}

private: //disabled

	Array(const Array&) {}
	Array& operator = (const Array&) { return *this; }
};


/** Array iterator, usage:
	@code
	// Array<T> a;
	for(Array_iterator<T> i(a); i.has_next(); ) {
		T& element=i.next();
		...
	}	
	@endcode
*/
template<typename T> class Array_iterator {

	const Array<T>& farray;
	T *fcurrent;
	T *flast;

public:

	Array_iterator(const Array<T>& aarray): farray(aarray) {
		fcurrent=farray.felements;
		flast=farray.felements+farray.count();
	}

	/// there are still elements
	bool has_next() {
		return fcurrent<flast;
	}

	/// quickly extracts next Array element
	T next() {
		return *(fcurrent++);
	}

};
#endif
