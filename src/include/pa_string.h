/** @file
	Parser: string class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STRING_H
#define PA_STRING_H

static const char* IDENT_STRING_H="$Date: 2003/07/24 11:31:21 $";

// includes

#include "pa_types.h"
#include "pa_array.h"

extern "C" { // cord's author forgot to do that
#define CORD_NO_IO
#include "cord.h"
};

// forwards

class Charset;
class Table;
class SQL_Connection;
class Dictionary;
class Request_charsets;
class String;
typedef Array<const String*> ArrayString;

/// this is result of pos functions which mean that substr were not found
#define STRING_NOT_FOUND ((size_t)-1)

class StringBody {

	CORD body;

public:

	StringBody(): body(CORD_EMPTY) {}
	StringBody(CORD abody): body(abody) {
		assert(!body // no body
			|| *body // ordinary string
			|| body[1]==1 // CONCAT_HDR
			|| body[1]==4 // FN_HDR 
			|| body[1]==6 // SUBSTR_HDR 
			);
	}
	/// WARNING: length is only HELPER length, str in ANY case should be zero-terminated
	StringBody(const char* str, size_t helper_length): body(CORD_EMPTY) {
		append_know_length(str, helper_length?helper_length:strlen(str));
	}
	static StringBody Format(int value);

	void clear() { body=CORD_EMPTY; }

	bool operator! () const { return is_empty(); }

	uint hash_code() const;

	const char* cstr() const { return CORD_to_const_char_star(body); }
	char* cstrm() const { return CORD_to_char_star(body); }

	size_t length() const { return CORD_len(body); }

	bool is_empty() const { return body==CORD_EMPTY; }

	void append_know_length(const char *str, size_t known_length) {
		if(known_length)
			body=CORD_cat_char_star(body, str, known_length);
	}
	void append_strdup_know_length(const char* str, size_t known_length) {
		if(known_length)
			append_know_length(pa_strdup(str, known_length), known_length);
	}
	void append(char c) { body=CORD_cat_char(body, c); }
	StringBody& operator << (const StringBody src) { body=CORD_cat(body, src.body); return *this; }
	StringBody& operator << (const char* str) { append_know_length(str, strlen(str)); return *this; }

	// could not figure out why this operator is needed [should do this chain: string->simple->==]
	bool operator < (const StringBody src) const { return CORD_cmp(body, src.body)<0; }
	bool operator > (const StringBody src) const { return CORD_cmp(body, src.body)>0; }
	bool operator <= (const StringBody src) const { return CORD_cmp(body, src.body)<=0; }
	bool operator >= (const StringBody src) const { return CORD_cmp(body, src.body)>=0; }
	bool operator != (const StringBody src) const { return CORD_cmp(body, src.body)!=0; }
	bool operator == (const StringBody src) const { return CORD_cmp(body, src.body)==0; }

	int ncmp(size_t x_begin, const StringBody y, size_t y_begin, size_t size) const {
		return CORD_ncmp(body, x_begin, y.body, y_begin, size);
	}

	char fetch(size_t index) const { return CORD_fetch(body, index); }
	StringBody mid(size_t index, size_t length) const { return CORD_substr(body, index, length); }
	size_t pos(const char* substr, size_t offset=0) const { return CORD_str(body, offset, substr); }
	size_t pos(const StringBody substr, size_t offset=0) const { 
		if(!substr.length())
			return STRING_NOT_FOUND; // in this case CORD_str returns 0 [parser users got used to -1]
		return CORD_str(body, offset, substr.body); 
	}
	size_t pos(char c, 
		size_t offset=0) const {
		return CORD_chr(body, offset, c);
	}

	template<typename I> void for_each(int (*callback)(const char* s, I), I info) const {
		CORD_iter5(body, 0, 0, (CORD_batched_iter_fn)callback, info);
	}

	void set_pos(CORD_pos& pos, size_t index) const { CORD_set_pos(pos, body, index); }

	StringBody normalize() const {
		return StringBody(CORD_balance(body));
	}

	void dump() const {
		CORD_dump(body);
	}
};

/** 
	String which knows the language of all it's fragments.

	All pieces remember 
	- whether they are tainted or not, 
	  and the language which should be used to detaint them
*/
class String: public PA_Object {

//	friend class StringBody;

public:

	/** piece is tainted or not. the language to use when detaint
		remember to change String_Untaint_lang_name @ untaint.C along
	*/
	enum Language {
		L_UNSPECIFIED=0, ///< zero value handy for hash lookup @see untaint_lang_name2enum
		// these two must go before others, there are checks for >L_AS_IS
		L_CLEAN, ///< clean
		L_AS_IS,     ///< leave all characters intact

		L_PASS_APPENDED,
			/**<
				leave language built into string being appended.
				just a flag, that value not stored
			*/
		L_TAINTED,  ///< tainted, untaint language as assigned later 
		// untaint languages. assigned by ^untaint[lang]{...}
		L_FILE_SPEC, ///< file specification
		L_HTTP_HEADER,    ///< text in HTTP response header
		L_MAIL_HEADER,    ///< text in mail header
		L_URI,       ///< text in uri
		L_TABLE,     ///< ^table:set body
		L_SQL,       ///< ^table:sql body
		L_JS,        ///< JavaScript code
		L_XML,		///< ^dom:set xml
		L_HTML,      ///< HTML code (for editing)
		L_OPTIMIZE_BIT = 0x8000  ///< flag, requiring cstr whitespace optimization
	};

	struct Fragment { 
		Language lang; ///< untaint flag, later untaint language
		size_t length;  ///< length
		Fragment(Language alang, size_t asize): lang(alang), length(asize) {
			assert(alang!=L_UNSPECIFIED);
			assert(asize!=0);
			assert(asize!=(size_t)-1);
		}
	};

	class ArrayFragment: public Array<Fragment> {
		void append(element_type src) {
			*static_cast<Array<Fragment> *>(this)+=src;
		}
		/// hiding from accidental USE, use append_positions
		void append(const ArrayFragment& src, int offset, int limit) { 
			static_cast<Array<Fragment> *>(this)->append(src, offset, limit);
		}
	public:
		ArrayFragment& operator += (element_type src) {
			if(size_t lcount=count()) { // not empty?
				// try to join with last
				Fragment& last=get_ref(lcount-1);
				if(last.lang==src.lang) {
					last.length+=src.length;
					return *this;
				}
			}
			append(src);
			return *this;
		}
		void append(const ArrayFragment& src) { append(src, 0, ARRAY_OPTION_LIMIT_ALL); }
		void append_positions(const ArrayFragment& src, size_t substr_begin, size_t substr_end);

		size_t length() {
			size_t result=0;
			for(Array_iterator<element_type> i(*this); i.has_next(); ) {
				const Fragment fragment=i.next();
				result+=fragment.length;
			}
			return result;
		}
	};

	struct C {
		const char *str;
		size_t length;
		operator const char *() { return str; }
		C(const char *astr, size_t asize): str(astr), length(asize) {}
	};

	struct Cm {
		char *str;
		size_t length;
		//operator char *() { return str; }
		Cm(char *astr, size_t asize): str(astr), length(asize) {}
	};

private:

	StringBody body; ///< all characters of string
	ArrayFragment fragments; ///< fragment language+length info

public:

	explicit String(const char* cstr=0, size_t helper_length=0, bool tainted=false);
	explicit String(const C cstr, bool tainted=false);
	String(const String& src);
	String(StringBody abody, Language alang): body(abody) {
		fragments+=Fragment(alang, abody.length());
	}

#define ASSERT_STRING_INVARIANT(string) \
	assert((string).body.length()==(string).fragments.length())

	/// for convinient hash lookup
	operator const StringBody() const { return body; }

	bool is_empty() const { return body.is_empty(); }
	size_t length() const { return body.length(); }

	/// convert to CORD. if 'lang' known, forcing 'lang' to it
	StringBody cstr_to_string_body(Language lang=L_AS_IS, 
		SQL_Connection* connection=0,
		const Request_charsets *charsets=0) const;

	/// convert to constant C string. if 'lang' known, forcing 'lang' to it
	const char* cstr(Language lang=L_AS_IS, 
		SQL_Connection* connection=0,
		const Request_charsets *charsets=0) const {
		return cstr_to_string_body(lang, connection, charsets).cstr();
	}
	/// convert to Modifiable C string. if 'lang' known, forcing 'lang' to it
	char *cstrm(Language lang=L_AS_IS, 
		SQL_Connection* connection=0,
		const Request_charsets *charsets=0) const {
		return cstr_to_string_body(lang, connection, charsets).cstrm();
	}
	/// puts pieces to buf
	Cm serialize(size_t prolog_size) const;
	/// appends pieces from buf to self
	bool deserialize(size_t prolog_size, void *buf, size_t buf_size);
	/// @see StringBody::append_know_length
	String& append_know_length(const char* str, size_t known_length, Language lang);
	/// @see StringBody::append_help_length
	String& append_help_length(const char* str, size_t helper_length, Language lang);
	String& append_strdup(const char* str, size_t helper_length, Language lang);

	bool operator == (const char* y) const { return body==StringBody(y); }
	bool operator != (const char* y) const { return body!=StringBody(y); }

	/// this starts with y
	bool starts_with(const char* y) const {
		return body.ncmp(0/*x_begin*/, StringBody(y), 0/*y_begin*/, strlen(y))==0;
	}
	/// x starts with this
	bool this_starts(const char* x) const {
		return StringBody(x).ncmp(0/*x_begin*/, body, 0/*y_begin*/, length())==0;
	}

	String& append_to(String& dest, Language lang, bool forced) const;
	String& append(const String& src, Language lang, bool forced=false) { 
		return src.append_to(*this, lang, forced);
	}
	String& operator << (const String& src) { return append(src, L_PASS_APPENDED); }
	String& operator << (const char* src) { return append_help_length(src, 0, L_AS_IS); }
	String& operator << (const StringBody src) { 
		body<<src;
		fragments+=Fragment(L_AS_IS, src.length());
		return *this;
	}

	/// extracts first char of a string, if any
	char first_char() const {
		return is_empty()?0:body.fetch(0);
	}

	bool operator < (const String& src) const { return body<src.body; }
	bool operator > (const String& src) const { return body>src.body; }
	bool operator <= (const String& src) const { return body<=src.body; }
	bool operator >= (const String& src) const { return body>=src.body; }
	bool operator != (const String& src) const { return body!=src.body; }
	bool operator == (const String& src) const { return body==src.body; }

	/// extracts [start, finish) piece of string
	String& mid(size_t substr_begin, size_t substr_end) const;

	/** 
		ignore lang if it's L_UNSPECIFIED
		but when specified: look for substring that lies in ONE fragment in THAT lang
		@return position of substr in string, -1 means "not found" [const char* version]
	*/
	size_t pos(const StringBody substr, 
		size_t this_offset=0, Language lang=L_UNSPECIFIED) const;
	/// String version of @see pos(const char*, int, Language)
	size_t pos(const String& substr, 
		size_t this_offset=0, Language lang=L_UNSPECIFIED) const;
	size_t pos(char c, 
		size_t this_offset=0) const {
		return body.pos(c, this_offset);
	}

	void split(ArrayString& result, 
		size_t& pos_after,
		const char* delim, 
		Language lang=L_UNSPECIFIED, int limit=-1) const;
	void split(ArrayString& result, 
		size_t& pos_after, 
		const String& delim, 
		Language lang=L_UNSPECIFIED, int limit=-1) const;

	typedef void (*Row_action)(Table& table, ArrayString* row, 
		int prestart, int prefinish, 
		int poststart, int postfinish,
		void *info);
	/**
		@return table of found items, if any.
		table format is defined and fixed[can be used by others]: 
		@verbatim
			prematch/match/postmatch/1/2/3/...
		@endverbatim
	*/
	Table* match(Charset& source_charset,
		const String& regexp, 
		const String* options,
		Row_action row_action, void *info,
		bool& just_matched) const;
	enum Change_case_kind {
		CC_UPPER,
		CC_LOWER
	};
	String& change_case(Charset& source_charset,
		Change_case_kind kind) const;
	const String& replace(const Dictionary& dict) const;
	double as_double() const;
	int as_int() const;

private: //disabled

	String& operator = (const String&) { return *this; }

};

/// simple hash code of string. used by Hash
inline uint hash_code(const StringBody self) {
	return self.hash_code();
}

#endif
