/** @file
	Parser: array class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_array.h,v 1.40 2001/05/16 16:54:00 parser Exp $
*/

#ifndef PA_ARRAY_H
#define PA_ARRAY_H

#include <stddef.h>

#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"

/**	
	Pooled Array.

	Internal structure:
	@verbatim
		Array               Chunk0
		======              ========
		head--------------->[ptr]
		append_here-------->[ptr]
		link_row            ........
				.			.
				.			[ptr]
				...........>[link to the next chunk]
	@endverbatim
*/

class Array : public Pooled {
public:

	/// Array item type
	typedef void Item;

	/*/// for_each iterator function type, const info
	typedef void (*For_each_func_const)(Item *value, const void *info);
	*/

	/// for_each iterator function type
	typedef void (*For_each_func)(Item *value, void *info);

	/// first_that iterator function type, const info
	typedef bool (*First_that_func_const)(Item *value, const void *info);

	/// first_that iterator function type
	typedef bool (*First_that_func)(Item *value, void *info);

	enum {
		CR_INITIAL_ROWS_DEFAULT=3, ///< default preallocated row count
		CR_GROW_COUNT=3 ///< each time the Array chunk_is_full() array expanded()
	};

public:

	Array(Pool& apool, int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	/**
		size Array. how many items are in it. 
		must be used with quick_get like this:
		@code
			int size=src.quick_size();
			for(int i=0; i<size; i++) {
				z=src.quick_get(i);
			}
		@endcode
	*/
	int quick_size() const { 
		// for quick_get
		cache_chunk_base=0;
		cache_chunk=head;
		return size(); 
	}
	/// size Array. how many items are in it
	int size() const { return fused_rows; }
	/// append Item to array
	Array& operator += (Item *src);
	/// append int value to array
	Array& operator += (int value) { return *this+=reinterpret_cast<Item *>(value); }

	/// dirty hack to allow constant items storage. I long for Array<const Item*>
	Array& operator += (const Item *src) { return *this+=const_cast<Item *>(src); }

	/// append other Array portion to this one. starting from offset
	Array& append_array(const Array& src, int offset=0);

	/** 
		quickly gets some item considering...

		these true:
			- index increments from 0 to size()-1
			- index>=0 && index<size()
			- index>=cache_chunk_base
	*/
	Item *quick_get(int index) const {
		// next chunk will be with "index" row
		if(!(index<cache_chunk_base+cache_chunk->count)) {
			int count=cache_chunk->count;
			cache_chunk_base+=count;
			cache_chunk=cache_chunk->rows[count].link;
		}
		
		return cache_chunk->rows[index-cache_chunk_base].item;
	}

	Item *get(int index) const;
	int get_int(int index) const { return reinterpret_cast<int>(get(index)); }

	void put(int index, Item *item);
	/// convinient way to get strings from Array. I long for Array<const String *>
	const String *get_string(int index) const { 
		return const_cast<const String *>(static_cast<String *>(get(index))); 
	}
	const String *quick_get_string(int index) const { 
		return const_cast<const String *>(static_cast<String *>(quick_get(index))); 
	}

	/*/// iterate over all elements, const info
	void for_each(For_each_func_const func, const void *info=0) const;
	/*/

	/// iterate over all elements
	void for_each(For_each_func func, void *info=0) const;

	/// iterate over all elements until condition, const info
	void* first_that(First_that_func_const func, const void *info=0) const;

	/// iterate over all elements until condition
	void* first_that(First_that_func func, void *info=0) const;

private:

	/// several record elements
	struct Chunk {
		int count;  ///< the number of rows in chunk
		/// item or a link to next chunk union
		union Row {
			Item *item;
			Chunk *link;  ///< link to the next chunk in chain
		} rows[1];
		// next rows are here
	}
		*head;  ///< the head chunk of the chunk chain

	/** last allocated chunk
		helps appending Arrays
	*/
	Chunk *tail;

	/// next append would write to this record
	Chunk::Row *append_here;
	
	/**	the address of place where lies address 
		of the link to the next chunk to allocate
	*/
	Chunk::Row *link_row;

private:

	// array size
	int fused_rows;

	mutable int cache_chunk_base;
	mutable Chunk *cache_chunk;
	
private:

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand(int chunk_rows);

private: //disabled

	//Array(Array&) { }
	Array& operator = (const Array&) { return *this; }
};

#endif
