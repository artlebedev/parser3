/*
  $Id: pa_array.h,v 1.11 2001/01/29 20:10:31 paf Exp $
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
#include "pa_string.h"

class Pool;

class Array {
public:

	typedef void Item;

	enum {
		CR_INITIAL_ROWS_DEFAULT=10,
		CR_GROW_PERCENT=60
	};

public:

	void *operator new(size_t size, Pool& apool);
	Array(Pool& apool, int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	int size() { return fused_rows; }
	Array& operator += (Item *src);
	Array& append_array(Array& src);
	Item *get(int index);
	char *get_cstr(int index) { return static_cast<char *>(get(index)); }
	String *get_string(int index) { return static_cast<String *>(get(index)); }

protected:

	// the pool I'm allocated on
	Pool& pool;

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

	int cache_chunk_base;
	Chunk *cache_chunk;
	
private:

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand(int chunk_rows);

private: //disabled

	//Array(Array&) { }
	Array& operator = (Array&) { return *this; }
};

#endif
