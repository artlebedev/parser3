/** @file
	Parser: array class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_array.h,v 1.29 2001/03/24 10:54:44 paf Exp $
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

	/// for_each iterator function type
	typedef void (*For_each_func)(Item *value, void *info);

	/// first_that iterator function type
	typedef Item *(*First_that_func)(Item *value, const void *info);

	enum {
		CR_INITIAL_ROWS_DEFAULT=10, ///< default preallocated row count
		CR_GROW_PERCENT=60 ///< each time the Array chunk_is_full() array expanded()
	};

public:

	Array(Pool& apool, int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	/// size Array. how many items are in it
	int size() const { 
		// for get and quick_get
		cache_chunk_base=0;
		cache_chunk=head;
		return fused_rows; 
	}
	/// append Item to array
	Array& operator += (Item *src);

	/// dirty hack to allow constant items storage. I long for Array<const Item*>
	Array& operator += (const Item *src) { return *this+=const_cast<Item*>(src); }

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
	void put(int index, Item *item);
	/// convinient way to get strings from Array. I long for Array<const String *>
	const String *get_string(int index) const { 
		return const_cast<const String *>(static_cast<String *>(get(index))); 
	}

	/// iterate over all elements
	void for_each(For_each_func func, void *info=0);

	/// iterate over all elements until condition
	void* first_that(First_that_func func, const void *info=0);

private:

	struct Chunk {
		// the number of rows in chunk
		int count;
		union Row {
			Item *item;
			Chunk *link;  // link to the next chunk in chain
		} rows[1];
		// next rows are here
	}
		*head;  // the head chunk of the chunk chain

	// last allocated chunk
	// helps appending Arrays
	Chunk *tail;

	// next append would write to this record
	Chunk::Row *append_here;
	
	// the address of place where lies address 
	// of the link to the next chunk to allocate
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
