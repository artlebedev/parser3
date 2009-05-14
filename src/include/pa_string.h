/** @file
	Parser: string class decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STRING_H
#define PA_STRING_H

static const char * const IDENT_STRING_H="$Date: 2009/05/14 08:10:09 $";

// includes
#include "pa_types.h"
#include "pa_array.h"

extern "C" { // cord's author forgot to do that
#define CORD_NO_IO
#include "cord.h"
};

// defines

// cord extension
/* Returns true if x does contain                                       */
/* char not_c at positions i..i+n. Value i,i+n must be < CORD_len(x).	*/
int CORD_range_contains_chr_greater_then(CORD x, size_t i, size_t n, int c);
size_t CORD_block_count(CORD x);

// forwards

class Charset;
class Table;
class SQL_Connection;
class Dictionary;
class Request_charsets;
class String;
typedef Array<const String*> ArrayString;
class VRegex;

// generally useful

int pa_atoi(const char* str, const String* problem_source=0);
double pa_atod(const char* str, const String* problem_source=0);

/// this is result of pos functions which mean that substr were not found
#define STRING_NOT_FOUND ((size_t)-1)

template<typename T>
inline size_t get_length(T current) {
	return current;
}

/** 
	String which knows the lang of all it's langs.

	All pieces remember 
	- whether they are tainted or not, 
	  and the lang which should be used to detaint them
*/


class String: public PA_Object {
public:

	/** piece is tainted or not. the lang to use when detaint
		remember to change String_Untaint_lang_name @ untaint.C along

		WARNING WARNING WARNING WARNING WARNING WARNING 
		
		pos function compares(<=) languages, that is used in searching
		for table column separator being L_CLEAN or L_AS_IS.
		they search for AS_IS, meaning AS_IS|CLEAN [doing <=L_AS_IS check].
		
		letters assigned for debugging, but it's important for no language-letter
		come before L_AS_IS other then L_CLEAN

		WARNING WARNING WARNING WARNING WARNING WARNING 
	*/
	enum Language {
		L_UNSPECIFIED=0, ///< no real string has parts of this lange: it's just convinient to check when string's empty
		// these two must go before others, there are checks for >L_AS_IS
		L_CLEAN='0',	///< clean  WARNING: read above warning before changing
		L_AS_IS='A',	///< leave all characters intact  WARNING: read above warning before changing

		L_PASS_APPENDED='P',
			/**<
				leave lang built into string being appended.
				just a flag, that value not stored
			*/
		L_TAINTED='T',	///< tainted, untaint lang as assigned later 
		// untaint langs. assigned by ^untaint[lang]{...}
		L_FILE_SPEC='F',	///< file specification
		L_HTTP_HEADER='h',	///< text in HTTP response header
		L_MAIL_HEADER='m',	///< text in mail header
		L_URI='U',		///< text in uri
		L_SQL='Q',		///< ^table:sql body
		L_JS='J',		///< JavaScript code
		L_XML='X',		///< ^dom:set xml
		L_HTML='H',		///< HTML code
		L_REGEX='R',	///< RegEx expression
		L_HTTP_COOKIE='C',	///< cookies encoded as %uXXXX for compartibility with js functions encode/decode
		L_FILE_POST='f', ///temporary escaping zero-char
		// READ WARNING ABOVE BEFORE ADDING ANYTHING
		L_OPTIMIZE_BIT = 0x80  ///< flag, requiring cstr whitespace optimization
	};

	enum Trim_kind {
		TRIM_BOTH,
		TRIM_START,
		TRIM_END
	};


	union Languages {

		struct {
#ifdef PA_LITTLE_ENDIAN
			Language lang:8;
			size_t is_not_just_lang:sizeof(CORD)*8-8;
#elif defined(PA_BIG_ENDIAN)
			size_t is_not_just_lang:sizeof(CORD)*8-8;
			Language lang:8;
#else
#	error word endianness not determined for some obscure reason
#endif
		} opt;
		CORD langs;

		template<typename C>
		CORD make_langs(C current) const {
			return opt.is_not_just_lang?
				langs
				:CORD_chars((char)opt.lang, get_length(current));
		}

		CORD make_langs(size_t aoffset, size_t alength)  const {
			return opt.is_not_just_lang?
				CORD_substr(langs, aoffset, alength)
				:CORD_chars((char)opt.lang, alength);
		}

		/// appending when 'langs' already contain something [simple cases handled elsewhere]
		template<typename C>
		void append(C current, 
			const CORD to_nonempty_target_langs) {
			assert(langs);

			if(opt.is_not_just_lang)
				langs=CORD_cat(langs, to_nonempty_target_langs);
			else { // we were "just lang"
				size_t current_size=get_length(current);
				assert(current_size);
				langs=CORD_cat(
					CORD_chars((char)opt.lang, current_size),  // first piece [making from just 'lang']
					to_nonempty_target_langs); // new piece
			}
		}

	public:

		const char* v() const;
		void dump() const;

		Languages(): langs(0) {}
		Languages(Language alang) {
			opt.lang=alang;
			opt.is_not_just_lang=0;
		}

		/// MUST be called exactly prior to modification of current [uses it's length]
		template<typename C>
		void append(C current, Language alang, size_t asize) {
			assert(alang);
			assert(asize);

			if(!opt.is_not_just_lang)
				if(opt.lang) {
					if(opt.lang==alang) // same language? ignoring
						return;
				} else {
					opt.lang=alang; // to uninitialized
					return;
				}

			append(current, CORD_chars((char)alang, asize));
		}

		template<typename C>
		void appendHelper(C current, Language alang, C asize_helper) {
			assert(alang);

			if(!opt.is_not_just_lang)
				if(opt.lang) {
					if(opt.lang==alang) // same length? ignoring
						return;
				} else {
					opt.lang=alang; // to uninitialized
					return;
				}

			append(current, CORD_chars((char)alang, asize_helper.length()));
		}

		template<typename C>
		void appendHelper(C current, C length_helper, const Languages src) {

			if(!langs)
				langs=src.langs; // to uninitialized
			else if(!src.opt.is_not_just_lang)
				appendHelper(current, src.opt.lang, length_helper); // simplifying when simple source
			else
				append(current, src.make_langs(length_helper));
		}


		/// MUST be called exactly prior to modification of current [uses it's length]
		template<typename C>
		void append(C current,
			const Languages src, size_t aoffset, size_t alength) {
			assert(alength);

			if(!langs) // to uninitialized?
				if(src.opt.is_not_just_lang)
					langs=CORD_substr(src.langs, aoffset, alength); // to uninitialized complex
				else
					opt.lang=src.opt.lang; // to uninitialized simple
			else 
				if(!opt.is_not_just_lang && !src.opt.is_not_just_lang && opt.lang==src.opt.lang) // both simple & of same language?
					return; // ignoring
				else
					append(current, src.make_langs(aoffset, alength));
		}

		/// checks if we have lang<=alang all from aoffset to aoffset+alength
		bool check_lang(Language alang, size_t aoffset, size_t alength) const {
			if(alang==L_UNSPECIFIED) // ignore lang?
				return true;

			if(opt.is_not_just_lang)
				return CORD_range_contains_chr_greater_then(langs, aoffset, alength, (unsigned)alang)==0;
			else
				return (unsigned)opt.lang<=(unsigned)alang;
		}

		/// @returns count of blocks
		/// @todo currently there can be adjucent blocks of same language. someday merge them
		size_t count() const {
			return opt.is_not_just_lang?
				CORD_block_count(langs)
				: opt.lang?
					1
					: 0;
		};

		template<typename C, typename I> 
		void for_each(C current, 
			int callback(char, size_t, I), I info) const {
			
			if(opt.is_not_just_lang)
				CORD_block_iter(langs, 0, (CORD_block_iter_fn)callback, info);
			else
				callback(opt.lang, get_length(current), info);
		}

		bool invariant(size_t current_length) const {
			if(!langs)
				return current_length==0;
			if(opt.is_not_just_lang)
				return CORD_len(langs)==current_length;
			return true; // uncheckable, actually
		}
	};

	class Body {

		CORD body;

	public:

		const char* v() const;
		void dump() const;

		Body(): body(CORD_EMPTY) {}
		Body(CORD abody): body(abody) {
			assert(!body // no body
				|| *body // ordinary string
				|| body[1]==1 // CONCAT_HDR
				|| body[1]==4 // FN_HDR 
				|| body[1]==6 // SUBSTR_HDR 
				);
		}

		static Body Format(int value);

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
		Body& operator << (const Body src) { body=CORD_cat(body, src.body); return *this; }
		Body& operator << (const char* str) { append_know_length(str, strlen(str)); return *this; }

		// could not figure out why this operator is needed [should do this chain: string->simple->==]
		bool operator < (const Body src) const { return CORD_cmp(body, src.body)<0; }
		bool operator > (const Body src) const { return CORD_cmp(body, src.body)>0; }
		bool operator <= (const Body src) const { return CORD_cmp(body, src.body)<=0; }
		bool operator >= (const Body src) const { return CORD_cmp(body, src.body)>=0; }
		bool operator != (const Body src) const { return CORD_cmp(body, src.body)!=0; }
		bool operator == (const Body src) const { return CORD_cmp(body, src.body)==0; }

		int ncmp(size_t x_begin, const Body y, size_t y_begin, size_t size) const {
			return CORD_ncmp(body, x_begin, y.body, y_begin, size);
		}

		char fetch(size_t index) const { return CORD_fetch(body, index); }
		Body mid(size_t index, size_t length) const { return CORD_substr(body, index, length); }
		size_t pos(const char* substr, size_t offset=0) const { return CORD_str(body, offset, substr); }
		size_t pos(const Body substr, size_t offset=0) const { 
			if(substr.is_empty())
				return STRING_NOT_FOUND; // in this case CORD_str returns 0 [parser users got used to -1]

			// CORD_str checks for bad offset [CORD_chr does not]
			return CORD_str(body, offset, substr.body); 
		}
		size_t pos(char c, 
			size_t offset=0) const {
			if(offset>=length()) // CORD_chr does not check that [and ABORT's in that case]
				return STRING_NOT_FOUND;

			return CORD_chr(body, offset, c);
		}

		template<typename I> int for_each(
			int (*f)(char c, I), 
			I info) const {
			return CORD_iter(body, (CORD_iter_fn)f, (void*)info);
		}

		template<typename I> int for_each(
			int (*f1)(char c, I), 
			int (*f2)(const char* s, I), 
			I info) const {
			return CORD_iter5(body, 0, (CORD_iter_fn)f1, (CORD_batched_iter_fn)f2, info);
		}

		void set_pos(CORD_pos& pos, size_t index) const { CORD_set_pos(pos, body, index); }

		/*Body normalize() const {
			return Body(CORD_balance(body));
		}*/

		/// @returns this or 0 or mid. if returns this or 0 out_* are not filled
		Body trim(Trim_kind kind=TRIM_BOTH, const char* chars=0,
			size_t* out_start=0, size_t* out_length=0) const;
	};

	struct C {
		const char *str;
		size_t length;
		operator const char *() { return str; }
		C(): str(0), length(0) {}
		C(const char *astr, size_t asize): str(astr), length(asize) {}
	};

	struct Cm {
		char *str;
		size_t length;
		//operator char *() { return str; }
		Cm(): str(0), length(0) {}
		Cm(char *astr, size_t asize): str(astr), length(asize) {}
	};

private:

	Body body; ///< all characters of string
	Languages langs; ///< string characters lang

	const char* v() const;
	void dump() const;
	#define ASSERT_STRING_INVARIANT(string) \
		assert((string).langs.invariant((string).body.length()))

public:

	static const String Empty;

	explicit String(){};
	explicit String(const char* cstr, Language alang=L_CLEAN){
		if(cstr && *cstr){
			body=cstr;
			langs=alang;
		}
	}
	explicit String(const String::C cstr, Language alang=L_CLEAN){
		if(cstr.length){
			body=cstr.str;
			langs=alang;
		}
	}
	String(Body abody, Language alang): body(abody), langs(alang) {
		ASSERT_STRING_INVARIANT(*this);
	}
	String(const String& src): body(src.body), langs(src.langs) {
		ASSERT_STRING_INVARIANT(*this);
	}

	/// for convinient hash lookup
	operator const Body() const { return body; }

	bool is_empty() const { return body.is_empty(); }
	size_t length() const { return body.length(); }
	size_t length(Charset& charset) const;

	/// convert to CORD. if 'lang' known, forcing 'lang' to it
	Body cstr_to_string_body(Language lang=L_AS_IS, 
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
	/// @see Body::append_know_length
	String& append_know_length(const char* str, size_t known_length, Language lang);
	/// @see Body::append_help_length
	String& append_help_length(const char* str, size_t helper_length, Language lang);
	String& append_strdup(const char* str, size_t helper_length, Language lang);

	bool operator == (const char* y) const { return body==Body(y); }
	bool operator != (const char* y) const { return body!=Body(y); }

	/// this starts with y
	bool starts_with(const char* y) const {
		return body.ncmp(0/*x_begin*/, Body(y), 0/*y_begin*/, strlen(y))==0;
	}
	/// x starts with this
	bool this_starts(const char* x) const {
		return Body(x).ncmp(0/*x_begin*/, body, 0/*y_begin*/, length())==0;
	}

	String& append_to(String& dest, Language lang, bool forced) const;
	String& append(const String& src, Language lang, bool forced=false) { 
		return src.append_to(*this, lang, forced);
	}
	String& operator << (const String& src) { return append(src, L_PASS_APPENDED); }
	String& operator << (const char* src) { return append_help_length(src, 0, L_AS_IS); }
	String& operator << (const Body src);

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
	String& mid(Charset& charset, size_t from, size_t to, size_t helper_length=0) const;

	/** 
		ignore lang if it's L_UNSPECIFIED
		but when specified: look for substring that lies in ONE fragment in THAT lang
		@return position of substr in string, -1 means "not found" [const char* version]
	*/
	size_t pos(const Body substr, 
		size_t this_offset=0, Language lang=L_UNSPECIFIED) const;
	/// String version of @see pos(const char*, int, Language)
	size_t pos(const String& substr, 
		size_t this_offset=0, Language lang=L_UNSPECIFIED) const;
	size_t pos(char c, 
		size_t this_offset=0) const {
		return body.pos(c, this_offset);
	}
	size_t pos(Charset& charset,
		const String& substr, 
		size_t this_offset=0, Language lang=L_UNSPECIFIED) const;

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
	Table* match(VRegex* vregex,
		Row_action row_action, void *info,
		int& matches_count) const;
	enum Change_case_kind {
		CC_UPPER,
		CC_LOWER
	};
	String& change_case(Charset& source_charset,
		Change_case_kind kind) const;
	const String& replace(const Dictionary& dict) const;
	const String& trim(Trim_kind kind=TRIM_BOTH, const char* chars=0) const;
	double as_double() const { return pa_atod(cstr(), this); }
	int as_int() const { return pa_atoi(cstr(), this); }
	bool as_bool() const { return as_int()!=0; }
	const String& escape(Charset& source_charset) const;

private: //disabled

	String& operator = (const String&) { return *this; }

};

template<>
inline size_t get_length<String::Body>(String::Body body) {
	return body.length();
}

/// simple hash code of string. used by Hash
inline uint hash_code(const String::Body self) {
	return self.hash_code();
}


/// now that we've declared specialization we can use it
inline String& String::operator << (const String::Body src) { 
	langs.append(body, L_AS_IS, src.length());
	body<<src;
	return *this;
}


#endif
