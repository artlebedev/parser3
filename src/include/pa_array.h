/*
  $Id: pa_array.h,v 1.17 2001/02/21 17:36:29 paf Exp $
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

#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"

class Array : public Pooled {
public:

	typedef void Item;

	enum {
		CR_INITIAL_ROWS_DEFAULT=10,
		CR_GROW_PERCENT=60
	};

public:

	Array(Pool& apool, int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	int size() const { return fused_rows; }
	Array& operator += (Item *src);
	Array& append_array(const Array& src);
	Item *quick_get(int index) const {
		// considering these true:
		//   index increments from 0 to size()-1
		//   index>=0 && index<size()
		//   index>=cache_chunk_base

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
	const char *get_cstr(int index) const { 
		return static_cast<const char *>(get(index)); 
	}
	const String *get_string(int index) const { 
		return static_cast<const String *>(get(index)); 
	}

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
