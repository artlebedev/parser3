/** @file
	Parser: string class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_string.h,v 1.81 2001/04/20 14:18:36 paf Exp $
*/

#ifndef PA_STRING_H
#define PA_STRING_H

#include "pa_config_includes.h"

#include <string.h>
#include <stddef.h>

#include "pa_pool.h"
#include "pa_types.h"

class Table;

/**
	$MAIN:html-typo table elements must enlarge string not more that that
	that's a tradeoff - otherwise we'd have to scan string twice:
	- first for buffer length
	- second for replacements themselves
*/
#define UNTAINT_TIMES_BIGGER 10

#ifndef NO_STRING_ORIGIN
#	define STRING_APPEND_PARAMS \
		const char *src, size_t size,  \
		String::Untaint_lang lang, \
		const char *file, uint line
/// appends piece to String  @see String::real_append
#	define APPEND(src, size, lang, file, line) \
		real_append(src, size, lang, file, line)
#else
#	define STRING_APPEND_PARAMS \
		const char *src, \
		size_t size, \
		String::Untaint_lang lang
/// appends piece to String  @see String::real_append
#	define APPEND(src, size, lang, file, line) \
		real_append(src, size, lang)
#endif
/// appends clean piece to String  @see String::real_append
#define APPEND_CLEAN(src, size, file, line) \
	APPEND(src, size, String::UL_CLEAN, file, line)
/// appends tainted piece to String  @see String::real_append
#define APPEND_TAINTED(src, size, file, line) \
	APPEND(src, size, String::UL_TAINTED, file, line)
/// handy: appends const char* piece to String  @see String::real_append
#define	APPEND_CONST(src) APPEND_CLEAN(src, 0, 0, 0)

class Array;
class SQL_Connection;

/** 
	Pooled string.

	Internal structure:
	@verbatim
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
		UL_UNSPECIFIED=0, ///< zero value handy for hash lookup @see untaint_lang_name2enum
		UL_CLEAN, ///< clean
		UL_TAINTED,  ///< tainted, untaint language as assigned later 
		// untaint languages. assigned by ^untaint[lang]{...}
		UL_PASS_APPENDED,
			/**<
				leave language built into string being appended.
				just a flag, that value not stored
			*/
		UL_AS_IS,     ///< leave all characters intact
		UL_FILE_NAME, ///< filename
		UL_HTTP_HEADER,    ///< text in HTTP response header
		UL_MAIL_HEADER,    ///< text in mail header
		UL_URI,       ///< text in uri
		UL_TABLE,     ///< ^table:set body
		UL_SQL,       ///< ^table:sql body
		UL_JS,        ///< JavaScript code
		UL_HTML,      ///< HTML code (for editing)
		UL_HTML_TYPO  ///< HTML code with TYPOgraphic replacements (for showing)
	};

public:

	String(Pool& apool, const char *src=0, size_t src_size=0, bool tainted=false);
	String(const String& src);
	size_t size() const { return fsize; }
	/// convert to C string. if 'lang' known, forcing 'lang' to it
	char *cstr(Untaint_lang lang=UL_UNSPECIFIED, SQL_Connection *connection=0) const {
		char *result=(char *)malloc(size()*UNTAINT_TIMES_BIGGER+1);
		char *eol=store_to(result, lang, connection);
		*eol=0;
		return result;
	}
	/** append fragment
		@see APPEND_CLEAN, APPEND_TAINTED, APPEND_CONST
	*/
	String& real_append(STRING_APPEND_PARAMS);
	/// @return <0 ==0 or >0 depending on comparison result
	int cmp (int& partial, const String& src, 
		size_t this_offset=0, Untaint_lang lang=UL_UNSPECIFIED) const;
	bool operator < (const String& src) const {	int p; return cmp(p, src)<0; }
	bool operator > (const String& src) const {	int p; return cmp(p, src)>0; }
	bool operator <= (const String& src) const { int p; return cmp(p, src)<=0; }
	bool operator >= (const String& src) const { int p; return cmp(p, src)>=0; }
	bool operator == (const String& src) const {
		if(size()!=src.size()) // can speed up in trivial case
			return false;
		int p; return cmp(p, src)==0;
	}
	bool operator != (const String& src) const { int p; return cmp(p, src)!=0; }

	/**
		 @param partial 
			returns partial match status. 
			- -1: strings too different
			-  0: full match
			-  1: means @c this starts @c src
			-  2: means @src starts @this
	*/
	int cmp(int& partial, const char* src_ptr, size_t src_size=0, 
		size_t this_offset=0, Untaint_lang lang=UL_UNSPECIFIED) const;
	bool operator == (const char* src_ptr) const { 
		size_t src_size=src_ptr?strlen(src_ptr):0;
		if(size() != src_size)
			return false;
		int partial; // unused
		return cmp(partial, src_ptr, src_size)==0; 
	}
	bool operator != (const char* src_ptr) const { 
		int partial; // unused
		return cmp(partial, src_ptr, 0)!=0; 
	}


	/** 
		appends other String.

		marking all tainted pieces of it with @a lang.
		or marking ALL pieces of it with a @a lang when @a forced to.
	*/
	String& append(const String& src, Untaint_lang lang, bool forced=false);
	String& operator << (const String& src) { return append(src, UL_PASS_APPENDED); }
	String& operator << (const char *src) { return APPEND_CONST(src); }

	/// simple hash code of string. used by Hash
	uint hash_code() const;

	/// extracts [start, finish) piece of string
	String& mid(size_t start, size_t finish) const;

	/// @return position of substr in string, -1 means "not found" [String version]
	int pos(const String& substr, 
		size_t this_offset=0, Untaint_lang lang=UL_UNSPECIFIED) const;
	/// @return position of substr in string, -1 means "not found" [const char* version]
	int pos(const char *substr, size_t substr_size, 
		size_t this_offset=0, Untaint_lang lang=UL_UNSPECIFIED) const;

	void split(Array& result, 
		size_t *pos_after_ref, 
		const char *delim, size_t delim_size, 
		Untaint_lang lang, int limit=-1) const;
	void split(Array& result, 
		size_t *pos_after_ref, 
		const String& delim, 
		Untaint_lang lang, int limit=-1) const;

	typedef void (*Row_action)(Table& table, Array *row, int start, int finish, 
		void *info);
	bool match(const unsigned char *pcre_tables,
		const String *aorigin,		
		const String& regexp, 
		const String *options,
		Table **table,
		Row_action row_action, void *info) const;

#ifndef NO_STRING_ORIGIN
	/// origin of string. calculated by first row
	const Origin& origin() const;
#endif

private:

	struct Chunk {
		// the number of rows in chunk
		size_t count;
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

	/// convert to C string, store to 'dest' which must be big enough for proper untaint
	char *store_to(char *dest, 
		Untaint_lang lang=UL_UNSPECIFIED, SQL_Connection *connection=0) const;

private: //disabled

	String& operator = (const String&) { return *this; }

};

#endif
