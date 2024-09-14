/** @file
	Parser: Array & Array_iterator classes decls.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_ARRAY_H
#define PA_ARRAY_H

#define IDENT_PA_ARRAY_H "$Id: pa_array.h,v 1.92 2024/09/14 22:58:50 moko Exp $"

// includes

#include "pa_memory.h"
#include "pa_types.h"
#include "pa_exception.h"

// forwards

template<typename T> class Array_iterator;
template<typename T> class Array_reverse_iterator;

// defines

#define ARRAY_OPTION_LIMIT_ALL ((size_t)-1)

/// Simple Array
template<typename T> class Array: public PA_Object {

	friend class Array_iterator<T>;
	friend class Array_reverse_iterator<T>;

protected:

	/// elements[growing size] here
	T *felements;

	// allocated size
	size_t fallocated;

	// array size
	size_t fused;

public:
	typedef Array_iterator<T> Iterator;
	typedef Array_reverse_iterator<T> ReverseIterator;

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
			if(offset>=count)
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

	inline Array(size_t initial=0):
		fallocated(initial),
		fused(0)
	{
		felements=fallocated?(T *)pa_malloc(fallocated*sizeof(T)):0;
	}

#ifdef USE_DESTRUCTORS
	inline ~Array(){
		if(felements)
			pa_free(felements);
	}
#endif

	/// how many items are in Array
	inline size_t count() const { return fused; }

	/// append to array
	inline Array& operator+=(T src) {
		if(is_full())
			expand(fallocated>0? 2+fallocated/32 : 3); // 3 is PAF default, confirmed by tests

		felements[fused++]=src;

		return *this;
	}

	/// append other Array portion to this one. starting from offset
	Array& append(const Array& src, 
		size_t offset=0, 
		size_t limit=ARRAY_OPTION_LIMIT_ALL) { //< negative limit means 'all'. zero limit means 'nothing'

		size_t src_count=src.count();
		// skip tivials
		if(!src_count || !limit || offset>=src_count)
			return *this;
		// max(limit)
		size_t m=src_count-offset;
		// fix limit
		if(limit==ARRAY_OPTION_LIMIT_ALL || limit>m)
			limit=m;

		ssize_t delta=limit-(fallocated-fused);
		if(delta>0)
			expand(delta);

		T* from=&src.felements[offset];
		T* to=&felements[fused];
		for(T* from_end=from+limit; from<from_end; from++)
			*to++=*from;
		fused+=limit;
		return *this;
	}

	/// get index-element
	inline T get(size_t index) const {
		assert(index<count());
		return felements[index];
	}

	/// ref version of get
	inline T& get_ref(size_t index) const {
		assert(index<count());
		return felements[index];
	}

	/// put index-element
	inline void put(size_t index, T element) {
		assert(index<count());
		felements[index]=element;
	}

	/// insert index-element
	inline void insert(size_t index, T element) {
		assert(index<=count());

		if(is_full())
			expand(fallocated>0? 2+fallocated/32 : 3); // 3 is PAF default, confirmed by tests

		memmove(felements+index+1, felements+index, (fused-index) * sizeof(T));

		felements[index]=element;
		fused++;
	}

	/// remove index-element
	inline void remove(size_t index) {
		assert(index<count());
		if (index<--fused)
			memmove(felements+index, felements+index+1, (fused-index) * sizeof(T));
	}

	inline T operator [](size_t index) const { return get(index); }

	inline void clear() {
		if(fused)
			memset(felements, 0, fused * sizeof(T));
		fused=0;
	}

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

	inline T* ptr(size_t index){
		return felements + index;
	}

protected:

	bool is_full() {
		return fused == fallocated;
	}

	void expand(size_t delta) {
		if(fallocated){
			size_t new_allocated=fallocated+delta;
			felements=(T *)pa_realloc(felements, new_allocated*sizeof(T));
			fallocated=new_allocated;
		} else {
			fallocated=delta;
			felements=(T *)pa_malloc(fallocated*sizeof(T));
		}
	}

private: //disabled

	Array(const Array&) {}
	Array& operator = (const Array&) { return *this; }
};


/// Commonly used, templated to work with any integer type

template<typename T> char* pa_itoa(T n, T base=10){
	char buf[MAX_NUMBER + 1];
	char* pos=buf + MAX_NUMBER;
	*pos='\0';

	bool negative=n < 0;
	if (n < 0){
		n=-n;
	}

	do {
		*(--pos)=(n % base) + '0';
		n/=base;
	} while (n > 0);

	if (negative) {
		*(--pos) = '-';
	}
	return pa_strdup(pos, buf + MAX_NUMBER - pos);
}

template<typename T> char* pa_uitoa(T n, T base=10){
	char buf[MAX_NUMBER + 1];
	char* pos=buf + MAX_NUMBER;
	*pos='\0';

	do {
		*(--pos)=(n % base) + '0';
		n/=base;
	} while (n > 0);

	return pa_strdup(pos, buf + MAX_NUMBER - pos);
}


/** Array iterator, usage:
	@code
	// Array<T> a;
	for(Array_iterator<T> i(a); i; ) {
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
		flast=farray.felements + farray.fused;
	}

	/// there are still elements
	inline operator bool () {
		return fcurrent < flast;
	}

	/// returns the current element and advances the iterator
	inline T next() {
		return *(fcurrent++);
	}

	/// returns the current element
	inline T value() {
		return *(fcurrent);
	}

	// returns the current index of the iterator
	inline size_t index() {
		return fcurrent - farray.felements;
	}

	inline char *key(){
		return pa_uitoa(index());
	}

};

template<typename T> class Array_reverse_iterator {

	const Array<T>& farray;
	T *fcurrent;

public:

	Array_reverse_iterator(const Array<T>& aarray): farray(aarray) {
		fcurrent=farray.felements + farray.fused;
	}

	/// there are still elements
	inline operator bool () {
		return fcurrent > farray.felements;
	}

	/// returns the current element and advances the iterator
	inline T prev() {
		return *(--fcurrent);
	}

	// returns the current index of the iterator
	inline size_t index() {
		return fcurrent - farray.felements;
	}

	inline char *key(){
		return pa_uitoa(index());
	}

};
#endif
