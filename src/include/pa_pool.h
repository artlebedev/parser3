/*

	String				Chunk0
	======				========
	head_chunk--------->[ptr, size]
	append_here-------->[ptr, size]
	chunk_link_row      ........
			.			.
			.			[ptr, size]
			...........>[link to the next chunk]

*/

#ifndef PA_POOL_H
#define PA_POOL_H

#include <stddef.h>

class Pool;

class String {
	friend Pool;

	// the pool I'm allocated on
	Pool *pool;

	// the number of records per chunk
	int chunk_items;
	enum {
		CI_CONST_STRING_GROW=5,
		CI_DYNAMIC_STRING_GROW_DEFAULT=10
	};

	struct Chunk {
		union Row {
			// chunk item
			struct {
				char *ptr;  // pointer to the start of string fragment
				size_t size;  // length of the fragment
			} item;
			Chunk *link;  // link to the next chunk in chain
		} first;
		// next rows are here
	}
		*head_chunk;  // the head chunk of the chunk chain

	// next append would write to this record
	Chunk::Row *append_here;
	
	// the address of place where lies address 
	// of the link to the next chunk to allocate
	Chunk::Row *chunk_link_row;

	// new&constructors made private to enforce factory manufacturing at pool
	void *operator new(size_t size, Pool *apool);

	void construct(Pool *apool, int achunk_items);
	String(Pool *apool, int achunk_items=CI_DYNAMIC_STRING_GROW_DEFAULT) { 
		construct(apool, achunk_items); 
	}
	String(Pool *apool, char *src) {
		construct(apool, CI_CONST_STRING_GROW);
		*this+=src;
	}

	bool chunk_is_full() {
		return append_here == chunk_link_row;
	}
	void expand();

public:

	size_t size();
	char *c_str();
	String& operator += (char *src);
};

class Pool {
public:
	Pool();
	~Pool();
    void *alloc(size_t size);
    void *calloc(size_t size);

	String *makeString() { 
		return new(this) String(this);
	}
	String *makeString(int chunk_items) {
		return new(this) String(this, chunk_items);
	}
	String *makeString(char *src) {
		return new(this) String(this, src);
	}
};

#endif
