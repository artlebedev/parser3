/*
  $Id: pa_string.h,v 1.12 2001/01/29 20:46:22 paf Exp $
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

#ifdef HAVE_CONFIG_H
#include "pa_config.h"
#endif

#include <stddef.h>

#include "pa_types.h"

class Pool;

#ifndef NO_STRING_ORIGIN
#	define STRING_APPEND_PARAMS const char *src, char *file, uint line
#	define APPEND(src, file, line) real_append(src, file, line)
#else
#	define STRING_APPEND_PARAMS const char *src
#	define APPEND(src, file, line) real_append(src)
#endif


class String {
public:
	enum {
		CR_PREALLOCATED_COUNT=5,
		CR_GROW_PERCENT=60
	};

public:

	void *operator new(size_t size, Pool& apool);
	String(Pool& apool);
	String(String& src);
	size_t size() { return fsize; }
	int used_rows() { return fused_rows; }
	char *cstr();
	String& real_append(STRING_APPEND_PARAMS);
	bool operator == (String& src);

	uint hash_code();

protected:

	// the pool I'm allocated on
	Pool& pool;

private:

	struct Chunk {
		// the number of rows in chunk
		int count;
		union Row {
			// chunk item
			struct {
				const char *ptr;  // pointer to the start of string fragment
				size_t size;  // length of the fragment
				Origin origin;  // origin of this fragment
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

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand();

private: //disabled

	String& operator = (const String&) { return *this; }

};

#endif
