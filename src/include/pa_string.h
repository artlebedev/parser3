/*
  $Id: pa_string.h,v 1.15 2001/02/11 11:27:24 paf Exp $
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

#include "pa_pool.h"
#include "pa_types.h"

#ifndef NO_STRING_ORIGIN
#	define STRING_APPEND_PARAMS const char *src, char *file, uint line
#	define APPEND(src, file, line) real_append(src, file, line)
#else
#	define STRING_APPEND_PARAMS const char *src
#	define APPEND(src, file, line) real_append(src)
#endif

class String : public Pooled {
public:
	enum {
		CR_PREALLOCATED_COUNT=5,
		CR_GROW_PERCENT=60
	};

public:

	String(Pool& apool);
	String(const String& src);
	size_t size() const { return fsize; }
	int used_rows() const { return fused_rows; }
	char *cstr() const;
	String& real_append(STRING_APPEND_PARAMS);
	bool operator == (const String& src) const;

	uint hash_code() const;

	const Origin& origin() const { return head.rows[0].item.origin; }

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
