/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_string.h,v 1.29 2001/03/10 16:34:35 paf Exp $
*/

/*

	String				Chunk0
	======				========
	head--------------->[ptr, size, ...]
	append_here-------->[ptr, size, ...]
						.
						.
						[ptr, size, ...]
	link_row----------->[link to the next chunk]

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
#	define STRING_APPEND_PARAMS const char *src, size_t size, bool tainted, char *file, uint line
#	define APPEND(src, size, file, line) real_append(src, size, false, file, line)
#	define APPEND_TAINTED(src, size, file, line) real_append(src, size, true, file, line)
#else
#	define STRING_APPEND_PARAMS const char *src, size_t size, bool tainted
#	define APPEND(src, size, file, line) real_append(src, size, false)
#	define APPEND_TAINTED(src, size, file, line) real_append(src, size, true)
#endif
#define	APPEND_CONST(src) APPEND(src, 0, 0, 0)

class String : public Pooled {
public:
	enum {
		CR_PREALLOCATED_COUNT=5,
		CR_GROW_PERCENT=60
	};

	enum Untaint_lang {
		NO, // clean
		YES,  // tainted, untaint language as assigned later 
		// untaint languages. assigned by ^untaint[lang]{...}
		PASS_APPENDED,
			// leave language built into string being appended
			// just a flag, that value not stored
		AS_IS,
		SQL,
		JS,
		TABLE,
		HTML,
		HTML_TYPO
	};

public:

	String(Pool& apool);
	String(const String& src);
	size_t size() const { return fsize; }
	int used_rows() const { return fused_rows; }
	char *cstr() const;
	String& real_append(STRING_APPEND_PARAMS);
	int cmp (const String& src) const;
	bool operator < (const String& src) const {	return cmp(src)<0; }
	bool operator > (const String& src) const {	return cmp(src)>0; }
	bool operator <= (const String& src) const { return cmp(src)<=0; }
	bool operator >= (const String& src) const { return cmp(src)>=0; }
	bool operator == (const String& src) const {
		if(size()!=src.size()) // can speed up in trivial case
			return false;
		return cmp(src)==0;
	}
	bool operator != (const String& src) const { return cmp(src)!=0; }

	bool operator == (char* src) const;
	String& append(const String& src, Untaint_lang lang);

	uint hash_code() const;

	const Origin& origin() const { return head.rows[0].item.origin; }

private:

	struct Chunk {
		// the number of rows in chunk
		int count;
		union Row {
			// fragment
			struct { 
				const char *ptr;  // pointer to the start
				size_t size;  // length
				Untaint_lang lang; // untaint flag, later untaint language
#ifndef NO_STRING_ORIGIN
				Origin origin;  // origin
#endif
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
	// last chunk
	Chunk *last_chunk;

	// string size
	size_t fsize;

	// used rows in all chunks
	int fused_rows;

private:

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand();
	void set_lang(Chunk::Row *row, Untaint_lang lang, size_t size);

private: //disabled

	String& operator = (const String&) { return *this; }

};

#endif
