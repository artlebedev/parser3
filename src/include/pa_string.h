/*
  $Id: pa_string.h,v 1.5 2001/01/27 12:04:53 paf Exp $
*/

/*

	String				Chunk0
	======				========
	head--------->[ptr, size]
	append_here-------->[ptr, size]
	link_row      ........
			.			.
			.			[ptr, size]
			...........>[link to the next chunk]

*/

#ifndef PA_STRING_H
#define PA_STRING_H

#include <stddef.h>

#include "pa_types.h"

class Pool;

class String {
public:
	enum {
		CR_PREALLOCATED_COUNT=5,
		CR_GROW_PERCENT=60
	};

private:
	friend Pool;

	// the pool I'm allocated on
	Pool *pool;

	struct Chunk {
		// the number of rows per chunk
		int count;
		union Row {
			// chunk item
			struct {
				char *ptr;  // pointer to the start of string fragment
				size_t size;  // length of the fragment
			} item;
			Chunk *link;  // link to the next chunk in chain
		} rows[CR_PREALLOCATED_COUNT];
		// next rows are here
		Chunk *preallocated_link;
	}
		head;  // the head chunk of the chunk chain

	// next append would write to this record
	Chunk::Row *append_here;
	
	// the address of place where lies address 
	// of the link to the next chunk to allocate
	Chunk::Row *link_row;

private:
	// last chank allocated count
	int curr_chunk_rows;

	// string size
	size_t fsize;

	// used rows in all chunks
	int fused_rows;

private:
	// new&constructors made private to enforce factory manufacturing at pool
	void *operator new(size_t size, Pool *apool);

	void construct(Pool *apool);
	String(Pool *apool) { 
		construct(apool); 
	}
	String(Pool *apool, char *src) {
		construct(apool);
		*this+=src;
	}

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand();

private: //disabled

	String& operator = (String& src) { return *this; }

public:

	String(String& src);
	size_t size() { return fsize; }
	int used_rows() { return fused_rows; }
	char *c_str();
	String& operator += (char *src);
	bool operator == (String& src);

	uint hash_code();
};

#endif
