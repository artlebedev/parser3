/** @file
	Parser: string class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_string.h,v 1.130 2002/02/21 14:36:54 paf Exp $
*/

#ifndef PA_STRING_H
#define PA_STRING_H

#include "pa_pool.h"
#include "pa_types.h"

class Table;

#ifndef NO_STRING_ORIGIN
#	define STRING_APPEND_PARAMS \
		const char *src, size_t size,  \
		uchar lang, \
		const char *file, uint line
/// appends piece to String  @see String::real_append
#	define APPEND(src, size, lang, file, line) \
		real_append(src, size, lang, file, line)
#else
#	define STRING_APPEND_PARAMS \
		const char *src, \
		size_t size, \
		uchar lang
/// appends piece to String  @see String::real_append
#	define APPEND(src, size, lang, file, line) \
		real_append(src, size, lang)
#endif
/// appends clean piece to String @see String::real_append
#define APPEND_CLEAN(src, size, file, line) \
	APPEND(src, size, String::UL_CLEAN, file, line)
/// appends piece to String as-is @see String::real_append
#define APPEND_AS_IS(src, size, file, line) \
	APPEND(src, size, String::UL_AS_IS, file, line)
/// appends tainted piece to String  @see String::real_append
#define APPEND_TAINTED(src, size, file, line) \
	APPEND(src, size, String::UL_TAINTED, file, line)
/// handy: appends const char* piece to String  @see String::real_append
#define	APPEND_CONST(src) APPEND_AS_IS(src, 0, 0, 0)

class Array;
class SQL_Connection;
class Dictionary;

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
#include "pa_pragma_pack_begin.h"
class String : public Pooled {
public:

	enum {
		CR_PREALLOCATED_COUNT=2, ///< default preallocated item count
		CR_GROW_COUNT=1 ///< each time the String chunk_is_full() string expanded()
	};

	/** piece is tainted or not. the language to use when detaint
		remember to change String_Untaint_lang_name @ untaint.C along
	*/
	enum Untaint_lang {
		UL_UNSPECIFIED=0, ///< zero value handy for hash lookup @see untaint_lang_name2enum
		// these two must go before others, there are checks for >UL_AS_IS
		UL_CLEAN, ///< clean
		UL_AS_IS,     ///< leave all characters intact

		UL_PASS_APPENDED,
			/**<
				leave language built into string being appended.
				just a flag, that value not stored
			*/
		UL_TAINTED,  ///< tainted, untaint language as assigned later 
		// untaint languages. assigned by ^untaint[lang]{...}
		UL_FILE_SPEC, ///< file specification
		UL_HTTP_HEADER,    ///< text in HTTP response header
		UL_MAIL_HEADER,    ///< text in mail header
		UL_URI,       ///< text in uri
		UL_TABLE,     ///< ^table:set body
		UL_SQL,       ///< ^table:sql body
		UL_JS,        ///< JavaScript code
		UL_XML,		///< ^dom:set xml
		UL_HTML,      ///< HTML code (for editing)
		UL_OPTIMIZE_BIT = 0x80  ///< flag, requiring cstr whitespace optimization
	};

public:

	String(Pool& apool, const char *src=0, size_t src_size=0, bool tainted=false);
	String(const String& src);
	bool is_empty() const { return append_here==head.rows; }
	size_t size() const;
	/// convert to C string. if 'lang' known, forcing 'lang' to it
	char *cstr(Untaint_lang lang=UL_AS_IS, 
		SQL_Connection *connection=0,
		Charset *cstr_charset=0) const {

		char *result=(char *)malloc(cstr_bufsize(lang, connection, cstr_charset));
		char *eol=store_to(result, lang, connection, cstr_charset);
		*eol=0;
		return result;
	}
	char *cstr_debug_origins() const;
	/// puts pieces to buf
	void serialize(size_t prolog_size,  void *& buf, size_t& buf_size) const;
	/// appends pieces from buf to self
	void deserialize(size_t prolog_size, void *buf, size_t buf_size, const char *file);
	/** append fragment
		@see APPEND_AS_IS, APPEND_CLEAN, APPEND_TAINTED, APPEND_CONST
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

	String& append(const String& src, uchar lang, bool forced=false);
	String& operator << (const String& src) { return append(src, UL_PASS_APPENDED); }
	String& operator << (const char *src) { return APPEND_CONST(src); }

	/// simple hash code of string. used by Hash
	uint hash_code() const;

	/// extracts first char of a string
	char first_char() const;

	/// extracts [start, finish) piece of string
	String& mid(size_t start, size_t finish) const;

	/// @return position of substr in string, -1 means "not found" [String version]
	int pos(const String& substr, 
		int this_offset=0, Untaint_lang lang=UL_UNSPECIFIED) const;
	/// @return position of substr in string, -1 means "not found" [const char* version]
	int pos(const char *substr, size_t substr_size=0, 
		int this_offset=0, Untaint_lang lang=UL_UNSPECIFIED) const;

	void split(Array& result, 
		size_t *pos_after_ref, 
		const char *delim, size_t delim_size, 
		Untaint_lang lang=UL_UNSPECIFIED, int limit=-1) const;
	void split(Array& result, 
		size_t *pos_after_ref, 
		const String& delim, 
		Untaint_lang lang=UL_UNSPECIFIED, int limit=-1) const;

	typedef void (*Row_action)(Table& table, Array *row, int start, int finish, 
		void *info);
	/**
		@return true if fills table.
		table format is defined and fixed[can be used by others]: 
		@verbatim
			prematch/match/postmatch/1/2/3/...
		@endverbatim
	*/
	bool match(
		const String *aorigin,		
		const String& regexp, 
		const String *options,
		Table **table,
		Row_action row_action, void *info,
		bool *was_global=0) const;
	enum Change_case_kind {
		CC_UPPER,
		CC_LOWER
	};
	String& change_case(Pool& pool,
		Change_case_kind kind) const;
	String& replace(Pool& pool, Dictionary& dict) const;
	double as_double() const;
	int as_int() const;

#ifndef NO_STRING_ORIGIN
	/// origin of string. calculated by first row
	const Origin& origin() const;
#endif

private:

	/** several String fragments
		
		'mutable' because can write after it's end, after it was appended to somebody 
		@see String::append
	*/
	mutable struct Chunk {
		typedef uchar count_type;
		count_type count; ///< the number of rows in chunk
		/// string fragment or a link to next chunk union
		union Row {
			typedef uchar item_size_type;
			/// fragment
			struct { 
				const char *ptr;  ///< pointer to the start
				item_size_type size;  ///< length
				uchar/*Untaint_lang*/ lang; ///< untaint flag, later untaint language
#ifndef NO_STRING_ORIGIN
				Origin origin;  ///< origin
#endif
			} item;
			Chunk *link;  ///< link to the next chunk in chain
		} rows[CR_PREALLOCATED_COUNT+1/*for head.rows[CR_PREALLOCATED_COUNT].link*/];
	}
		head;  ///< the head chunk of the chunk chain

	/// next append would write to this record
	Chunk::Row *append_here;
	
private:
	/// last chunk
	mutable Chunk *last_chunk;

private:

	bool chunk_is_full() {
		return append_here == last_chunk->rows+last_chunk->count;
	}
	uint used_rows() const;
	void expand();
	
	Untaint_lang lang_of(size_t offset) const;

	size_t cstr_bufsize(Untaint_lang lang,
		SQL_Connection *connection,
		Charset *buf_charset) const;
	/// convert to C string, store to 'dest' which must be big enough for proper untaint
	char *store_to(char *dest, Untaint_lang lang=UL_UNSPECIFIED, 
		SQL_Connection *connection=0,
		Charset *store_to_charset=0) const;

	String& reconstruct(Pool& pool) const;
	void join_chain(Pool& pool, 
					   const Chunk*& achunk, const Chunk::Row*& arow, uint& acountdown, 
					   uchar& joined_lang, const char *& joined_ptr, size_t& joined_size) const;
	String& replace_in_reconstructed(Pool& pool, Dictionary& dict) const;

private: //disabled

	String& operator = (const String&) { return *this; }

};
#include "pa_pragma_pack_end.h"

#define STRING_PREPARED_FOREACH_ROW(self, body) \
	while(row!=(self).append_here) { \
		if(countdown==0) { \
			chunk=row->link; \
			row=chunk->rows; \
			countdown=chunk->count; \
		}; \
		countdown--;\
		{ body } \
		row++; \
	} 

#define STRING_PREFIX_FOREACH_ROW(self, body) { \
	const Chunk *chunk=&(self).head;  \
	const Chunk::Row *row=chunk->rows; \
	uint countdown=chunk->count; \
	STRING_PREPARED_FOREACH_ROW(self, body) \
}

#define STRING_FOREACH_ROW(body) STRING_PREFIX_FOREACH_ROW(*this, body)
#define STRING_SRC_FOREACH_ROW(body) STRING_PREFIX_FOREACH_ROW(src, body)

#endif
