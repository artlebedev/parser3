/*
  $Id: pa_array.h,v 1.3 2001/01/27 15:45:24 paf Exp $
*/

/*

	Array               Chunk0
	======              ========
	head--------------->[ptr]
	append_here-------->[ptr]
	link_row            ........
			.			.
			.			[ptr]
			...........>[link to the next chunk]

*/

#ifndef PA_ARRAY_H
#define PA_ARRAY_H

#include <stddef.h>

#include "pa_types.h"

class Pool;

class Array {
public:
	typedef void *Item;

	enum {
		CR_PREALLOCATED_COUNT=10,
		CR_GROW_PERCENT=60
	};

private:
	friend Pool;

	// the pool I'm allocated on
	Pool *pool;

	struct Chunk {
		// the number of rows in chunk
		int count;
		union Row {
			Item item;
			Chunk *link;  // link to the next chunk in chain
		} rows[1];
		// next rows are here
	}
		*head;  // the head chunk of the chunk chain

	// next append would write to this record
	Chunk::Row *append_here;
	
	// the address of place where lies address 
	// of the link to the next chunk to allocate
	Chunk::Row *link_row;

private:
	// last chank allocated count
	int curr_chunk_rows;

	// array size
	int fused_rows;

	int cache_chunk_base;
	Chunk *cache_chunk;
	
private:
	// new&constructors made private to enforce factory manufacturing at pool
	void *operator new(size_t size, Pool *apool);

	void construct(Pool *apool, int initial_rows);
	Array(Pool *apool) {
		construct(apool, CR_PREALLOCATED_COUNT); 
	}
	Array(Pool *apool, int initial_rows) {
		construct(apool, initial_rows); 
	}


	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand();

public:

	int size() { return fused_rows; }
	Array& operator += (Item src);

	/*
	void put(int index, Item item);
	Item get(int index);
	*/
	Item& operator [] (int index);

private: //disabled

	Array& operator = (Array& src) { return *this; }
	Array(Array& src) {}
};

#endif
