/** @file
	Parser: string class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_string.h,v 1.45 2001/03/19 20:46:36 paf Exp $
*/

#ifndef PA_STRING_H
#define PA_STRING_H

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#include <stddef.h>

#include "pa_pool.h"
#include "pa_types.h"

#define UNTAINT_TIMES_BIGGER 10

#ifndef NO_STRING_ORIGIN
#	define STRING_APPEND_PARAMS \
		const char *src, size_t size,  \
		bool tainted, \
		const char *file, uint line
/// appends clean piece to String
#	define APPEND(src, size, file, line) real_append(src, size, false, file, line)
/// appends tainted piece to String
#	define APPEND_TAINTED(src, size, file, line) real_append(src, size, true, file, line)
#else
#	define STRING_APPEND_PARAMS \
		const char *src, \
		size_t size, \
		bool tainted
/// appends clean piece to String
#	define APPEND(src, size, file, line) real_append(src, size, false)
/// appends tainted piece to String
#	define APPEND_TAINTED(src, size, file, line) real_append(src, size, true)
#endif
/// handy: appends const char* piece to String
#define	APPEND_CONST(src) APPEND(src, 0, 0, 0)

/** 
	Pooled string.

	Internal structure: @verbatim	

	String				Chunk0
	======				========
	head--------------->[ptr, size, ...]
	append_here-------->[ptr, size, ...]
						.
						.
						[ptr, size, ...]
	link_row----------->[link to the next chunk]

	@endverbatim

	All pieces remember 
	- the file and its line they are from [can be turned off by NO_STRING_ORIGIN]
	- whether they are tainted or not, 
	  and the language which should be used to detaint them
*/
class String : public Pooled {
public:
	enum {
		CR_PREALLOCATED_COUNT=5, ///< default preallocated item count
		CR_GROW_PERCENT=60 ///< each time the Array chunk_is_full() array expanded()
	};

	/// piece is tainted or not. the language to use when detaint 
	enum Untaint_lang {
		UNKNOWN=0, ///< when get by name fails
		NO, ///< clean
		YES,  ///< tainted, untaint language as assigned later 
		// untaint languages. assigned by ^untaint[lang]{...}
		PASS_APPENDED,
			/**<
				leave language built into string being appended.
				just a flag, that value not stored
			*/
		AS_IS,  ///< leave all characters intact
		FILE,   ///< filename
		HEADER, ///< text in response header
		URI,    ///< text in uri
		TABLE,  ///< ^table:set body
		SQL,    ///< ^table:sql body
		JS,     ///< JavaScript code
		HTML,   ///< HTML code (for editing)
		HTML_TYPO ///< HTML code with TYPOgraphic replacements (for showing)
	};

public:

	String(Pool& apool, const char *src=0, bool tainted=false);
	String(const String& src);
	size_t size() const { return fsize; }
	/// convert to C string
	char *cstr() const;
	String& real_append(STRING_APPEND_PARAMS);
	/// @return <0 ==0 or >0 depending on comparison result
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

	bool operator == (const char* b_ptr) const;
	/** 
		appends other String.

		marking all tainted pieces of it with \a lang.
		or marking ALL pieces of it with a \a lang when \a forced to.
	*/
	String& append(const String& src, Untaint_lang lang, bool forced=false);

	/// simple hash code of string. used by Hash
	uint hash_code() const;

#ifndef NO_STRING_ORIGIN
	/// origin of string. calculated by first row
	const Origin& origin() const;
#endif

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
	void set_lang(Chunk::Row *row, Untaint_lang lang, bool forced, size_t size);

private: //disabled

	String& operator = (const String&) { return *this; }

};

#endif
