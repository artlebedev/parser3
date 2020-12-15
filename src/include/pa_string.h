/** @file
	Parser: string class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_STRING_H
#define PA_STRING_H

#define IDENT_PA_STRING_H "$Id: pa_string.h,v 1.228 2020/12/15 17:10:32 moko Exp $"

// includes
#include "pa_types.h"
#include "pa_array.h"

extern "C" { // cord's author forgot to do that
#define CORD_NO_IO
#include "../lib/cord/include/cord.h"

#ifdef CORD_CAT_OPTIMIZATION
#define CORD_cat(x, y) CORD_cat_optimized(x, y)
#define CORD_cat_char_star(x, y, leny) CORD_cat_char_star_optimized(x, y, leny)
#endif
}

// defines

// cache hash code in String::Body for faster hash access
#define HASH_CODE_CACHING

// cache String::Body.length() for char* strings only, CORDs have own length
#define STRING_LENGTH_CACHING


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

double pa_atod(const char* str, const String* problem_source=0);
int pa_atoi(const char* str, int base=10, const String* problem_source=0);
unsigned int pa_atoui(const char *str, int base=10, const String* problem_source=0);
uint64_t pa_atoul(const char *str, int base=10, const String* problem_source=0);

/// this is result of pos functions which mean that substr were not found
#define STRING_NOT_FOUND ((size_t)-1)

/// CORD can't be empty string, thus checking it in assigment
#define AS_CORD(v) ((v) && *(v) ? (CORD)(v):0)

/** 
	String which knows the lang of all it's langs.

	All pieces remember 
	- whether they are tainted or not, 
	  and the lang which should be used to detaint them
*/


class String: public PA_Object {
	friend class StringSplitHelper;
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

#if _MSC_VER >= 1900
	/// required for VS2015+ to make sizeof(Languages::opt) == sizeof(CORD), will be 16 byte under x64 without it
	enum Language : size_t {
#else
	enum Language {
#endif
		L_UNSPECIFIED=0,	///< no real string has parts of this lange: it's just convinient to check when string's empty
		// these two must go before others, there are checks for >L_AS_IS
		L_CLEAN='0',		///< clean  WARNING: read above warning before changing
		L_AS_IS='A',		///< leave all characters intact  WARNING: read above warning before changing

		L_TAINTED='T',		///< tainted, untaint lang as assigned later
		// untaint langs. assigned by ^untaint[lang]{...}
		L_FILE_SPEC='F',	///< file specification
		L_HTTP_HEADER='h',	///< text in HTTP response header
		L_MAIL_HEADER='m',	///< text in mail header
		L_URI='U',		///< text in uri
		L_SQL='Q',		///< ^table:sql body
		L_JS='J',		///< JavaScript code
		L_XML='X',		///< ^xdoc:create xml
		L_HTML='H',		///< HTML code
		L_REGEX='R',		///< RegExp
		L_JSON='S',		///< JSON code
		L_HTTP_COOKIE='C',	///< cookies encoded as %uXXXX for compartibility with js functions encode/decode
		L_PARSER_CODE='p',	///< ^process body
		// READ WARNING ABOVE BEFORE ADDING ANYTHING
		L_OPTIMIZE_BIT = 0x80  ///< flag, requiring cstr whitespace optimization
	};

	enum Trim_kind {
		TRIM_BOTH,
		TRIM_START,
		TRIM_END
	};

	class Body;

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

		CORD make_langs(const Body& current) const {
			return opt.is_not_just_lang?langs:CORD_chars((char)opt.lang, current.length());
		}

		CORD make_langs(size_t aoffset, size_t alength)  const {
			return opt.is_not_just_lang?
				CORD_substr(langs, aoffset, alength, 0)
				:CORD_chars((char)opt.lang, alength);
		}

		/// appending when 'langs' already contain something [simple cases handled elsewhere]
		void append(size_t current, const CORD to_nonempty_target_langs) {
			assert(langs);
			if(opt.is_not_just_lang)
				langs=CORD_cat(langs, to_nonempty_target_langs);
			else {
				assert(current);
				langs=CORD_cat(CORD_chars((char)opt.lang, current), to_nonempty_target_langs);
			}
		}

		void append(const Body& current, const CORD to_nonempty_target_langs) {
			assert(langs);
			if(opt.is_not_just_lang)
				langs=CORD_cat(langs, to_nonempty_target_langs);
			else {
				size_t current_size=current.length();
				assert(current_size);
				langs=CORD_cat(CORD_chars((char)opt.lang, current_size), to_nonempty_target_langs);
			}
		}

	public:

		const char* visualize() const;
		void dump() const;

		Languages(): langs(0) {}
		Languages(Language alang) {
			opt.lang=alang;
			opt.is_not_just_lang=0;
		}

		/// MUST be called exactly prior to modification of current [uses it's length]
		void append(size_t current, Language alang, size_t length) {
			assert(alang);
			assert(length);

			if(!opt.is_not_just_lang) {
				if(opt.lang) {
					if(opt.lang==alang) // same language? ignoring
						return;
				} else {
					opt.lang=alang; // to uninitialized
					return;
				}
			}

			append(current, CORD_chars((char)alang, length));
		}

		void append(const Body &current, Language alang, size_t length) {
			assert(alang);
			assert(length);

			if(!opt.is_not_just_lang) {
				if(opt.lang) {
					if(opt.lang==alang) // same language? ignoring
						return;
				} else {
					opt.lang=alang; // to uninitialized
					return;
				}
			}

			append(current, CORD_chars((char)alang, length));
		}

		void appendHelper(const Body& current, Language alang, const Body &length_helper) {
			assert(alang);

			if(!opt.is_not_just_lang) {
				if(opt.lang) {
					if(opt.lang==alang) // same language? ignoring
						return;
				} else {
					opt.lang=alang; // to uninitialized
					return;
				}
			}

			append(current, CORD_chars((char)alang, length_helper.length()));
		}

		void appendHelper(const Body& current, const Languages &src, const Body& length_helper) {
			if(!langs){
				langs=src.langs; // to uninitialized
#ifdef CORD_CAT_OPTIMIZATION
				if(opt.is_not_just_lang && !CORD_IS_STRING(langs))
					CORD_concatenation_protect(langs);
#endif
			}
			else if(!src.opt.is_not_just_lang)
				appendHelper(current, src.opt.lang, length_helper); // simplifying when simple source
			else
				append(current, src.make_langs(length_helper));
		}

		/// MUST be called exactly prior to modification of current [uses it's length]
		void append(const Body& current, const Languages src, size_t aoffset, size_t alength) {
			assert(alength);

			if(!langs) // to uninitialized?
				if(src.opt.is_not_just_lang)
					langs=CORD_substr(src.langs, aoffset, alength, 0); // to uninitialized complex
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

		template<typename I> 
		void for_each(size_t current, int callback(char, size_t, I), I info) const {
			if(opt.is_not_just_lang)
				CORD_block_iter(langs, 0, (CORD_block_iter_fn)callback, info);
			else
				callback(opt.lang, current, info);
		}

		template<typename I> 
		void for_each(const Body& current, int callback(char, size_t, I), I info) const {
			if(opt.is_not_just_lang)
				CORD_block_iter(langs, 0, (CORD_block_iter_fn)callback, info);
			else
				callback(opt.lang, current.length(), info);
		}

		bool invariant(size_t current_length) const {
			if(!langs)
				return current_length==0;
			if(opt.is_not_just_lang)
				return CORD_len(langs)==current_length;
			return true; // uncheckable, actually
		}
	};

	struct C {
		const char *str;
		size_t length;
		C(): str(0), length(0) {}
		C(const char *astr, size_t asize): str(astr), length(asize) {}
		explicit C(Body abody): str(abody.cstr()), length(abody.length()) {}
	};

	struct Cm {
		char *str;
		size_t length;
		Cm(): str(0), length(0) {}
		Cm(char *astr, size_t asize): str(astr), length(asize) {}
		explicit Cm(Body abody): str(abody.cstrm()), length(abody.length()) {}
	};

	class Body {

		CORD body;

#ifdef HASH_CODE_CACHING
		// cached hash code is not reseted on write operations as test shows 
		// that string body does not change after it is stored as a hash key
		mutable uint hash_code;
#define INIT_HASH_CODE(c) ,hash_code(c)
#define ZERO_HASH_CODE hash_code=0;
#else
#define INIT_HASH_CODE(c)
#define ZERO_HASH_CODE
#endif

#ifdef STRING_LENGTH_CACHING
		// cached length is reseted on modification, used only for char*, not CORD
		mutable size_t string_length;
#define INIT_LENGTH(l) ,string_length(l)
#define ZERO_LENGTH string_length=0;
#else
#define INIT_LENGTH(l)
#define ZERO_LENGTH
#endif

	public:

		void dump() const;

		Body(): body(CORD_EMPTY) INIT_HASH_CODE(0) INIT_LENGTH(0) {}
		Body(const char *abody): body(AS_CORD(abody)) INIT_HASH_CODE(0) INIT_LENGTH(0) {}
		Body(CORD abody, uint ahash_code): body(abody) INIT_HASH_CODE(ahash_code) INIT_LENGTH(0) {}
		explicit Body(C ac): body(AS_CORD(ac.str)) INIT_HASH_CODE(0) INIT_LENGTH(ac.length) {}
		explicit Body(CORD abody): body(abody) INIT_HASH_CODE(0) INIT_LENGTH(0) {
#ifdef CORD_CAT_OPTIMIZATION
			assert(!body // no body
				|| *body // ordinary string
				|| body[1]==1 // CONCAT_HDR
				|| body[1]==3 // CONCAT_HDR_READ_ONLY
				|| body[1]==4 // FN_HDR 
				|| body[1]==6 // SUBSTR_HDR 
				);
#else
			assert(!body // no body
				|| *body // ordinary string
				|| body[1]==1 // CONCAT_HDR
				|| body[1]==4 // FN_HDR 
				|| body[1]==6 // SUBSTR_HDR 
				);
#endif
		}


		static Body Format(int value);

		void clear() { ZERO_LENGTH ZERO_HASH_CODE body=CORD_EMPTY; }

		bool operator! () const { return is_empty(); }

		inline CORD get_cord() const { return body; }
		uint get_hash_code() const;

		// never null
		const char* cstr() const {
#ifdef STRING_LENGTH_CACHING
			string_length = length();
			if(string_length){
				const char *result=CORD_to_const_char_star(body, string_length);
				const_cast<Body*>(this)->body=(CORD)result;
				return result;
			}
#endif
			return CORD_to_const_char_star(body, length());
		}

		// never null
		char* cstrm() const { return CORD_to_char_star(body, length()); }

#ifdef STRING_LENGTH_CACHING
		void set_length(size_t alength){ string_length = alength; }
		size_t length() const { return body ? CORD_IS_STRING(body) ? string_length ? string_length : (string_length=strlen((const char *)body)) : CORD_len(body) : 0; }
#else
		size_t length() const { return CORD_len(body); }
#endif

		inline bool is_empty() const { return body==CORD_EMPTY; }

		void append_know_length(const char *str, size_t known_length) {
			if(known_length){
				if(body){
					body = CORD_cat_char_star(body, str, known_length);
					ZERO_LENGTH 
				} else {
					body=(CORD)str;
#ifdef STRING_LENGTH_CACHING
					string_length=known_length;
#endif
				}
			}
		}
		void append_strdup_know_length(const char* str, size_t known_length) {
			if(known_length)
				append_know_length(pa_strdup(str, known_length), known_length);
		}
		void append(char c) { ZERO_LENGTH body=CORD_cat_char(body, c); }
		Body& operator << (const Body src) { ZERO_LENGTH body=CORD_cat(body, src.body); return *this; }

		Body& operator << (const char* str) { append_know_length(str, strlen(str)); return *this; }

		bool operator < (const Body src) const { return CORD_cmp(body, src.body)<0; }
		bool operator > (const Body src) const { return CORD_cmp(body, src.body)>0; }
		bool operator <= (const Body src) const { return CORD_cmp(body, src.body)<=0; }
		bool operator >= (const Body src) const { return CORD_cmp(body, src.body)>=0; }

		bool operator != (const Body src) const { return CORD_cmp(body, src.body)!=0; }
		bool operator == (const Body src) const { return CORD_cmp(body, src.body)==0; }

		bool operator != (const char *src) const { return CORD_cmp(body, AS_CORD(src))!=0; }
		bool operator == (const char *src) const { return CORD_cmp(body, AS_CORD(src))==0; }

		int ncmp(size_t x_begin, const Body y, size_t y_begin, size_t size) const {
			return CORD_ncmp(body, x_begin, y.body, y_begin, size);
		}

		char fetch(size_t index) const { return CORD_fetch(body, index); }
		Body mid(size_t aindex, size_t alength) const { return Body(CORD_substr(body, aindex, alength, length())); }
		size_t pos(const char* substr, size_t offset=0) const { return CORD_str(body, offset, AS_CORD(substr), length()); }
		size_t pos(const Body substr, size_t offset=0) const { 
			if(substr.is_empty())
				return STRING_NOT_FOUND; // in this case CORD_str returns 0 [parser users got used to -1]

			// CORD_str checks for bad offset [CORD_chr does not]
			return CORD_str(body, offset, substr.body, length()); 
		}
		size_t pos(char c, size_t offset=0) const {
			if(offset>=length()) // CORD_chr does not check that [and ABORT's in that case]
				return STRING_NOT_FOUND;

			return CORD_chr(body, offset, c);
		}

		size_t strrpbrk(const char* chars, size_t left, size_t right) const;

		size_t rskipchars(const char* chars, size_t left, size_t right) const;

		template<typename I>
		int for_each(int (*f)(char c, I), I info) const {
			return CORD_iter(body, (CORD_iter_fn)f, (void*)info);
		}

		template<typename I>
		int for_each(int (*f1)(char c, I), int (*f2)(const char* s, I), I info) const {
			return CORD_iter5(body, 0, (CORD_iter_fn)f1, (CORD_batched_iter_fn)f2, info);
		}

		void set_pos(CORD_pos& pos, size_t index) const { CORD_set_pos(pos, body, index); }

		/// @returns this or 0 or mid. if returns this or 0 out_* are not filled
		Body trim(Trim_kind kind=TRIM_BOTH, const char* chars=0,
			size_t* out_start=0, size_t* out_length=0, Charset* source_charset=0) const;
	};

private:

	Body body; ///< all characters of string
	Languages langs; ///< string characters lang

	void dump() const;
	#define ASSERT_STRING_INVARIANT(string) \
		assert((string).langs.invariant((string).body.length()))

public:

	static const String Empty;

	explicit String(){};
	explicit String(const char* cstr, Language alang=L_CLEAN) : body(cstr){
		if(body.get_cord()){
			langs=alang;
		}
	}
	explicit String(C ac, Language alang=L_CLEAN) : body(ac){
		if(body.get_cord()){
			langs=alang;
		}
	}
	String(Body abody, Language alang): body(abody), langs(alang) {
		ASSERT_STRING_INVARIANT(*this);
	}
	String(const String& src): body(src.body), langs(src.langs) {
		ASSERT_STRING_INVARIANT(*this);
	}
	String(int value, const char *format);

	/// for convinient hash lookup
#ifdef HASH_CODE_CACHING
	operator const Body&() const { return body; }
#else
	operator const Body() const { return body; }
#endif

	bool is_empty() const { return body.is_empty(); }
	size_t length() const { return body.length(); }
	size_t length(Charset& charset) const;

	/// convert to CORD forcing lang tainting
	Body cstr_to_string_body_taint(Language lang, SQL_Connection* connection=0, const Request_charsets *charsets=0) const;
	/// convert to CORD with tainting dirty to lang
	Body cstr_to_string_body_untaint(Language lang, SQL_Connection* connection=0, const Request_charsets *charsets=0) const;

	/// from body
	const char* cstr() const { return body.cstr(); }
	char* cstrm() const { return body.cstrm(); }

	/// convert to constant C string forcing lang tainting
	const char* taint_cstr(Language lang, SQL_Connection* connection=0, const Request_charsets *charsets=0) const {
		return cstr_to_string_body_taint(lang, connection, charsets).cstr();
	}
	char *taint_cstrm(Language lang, SQL_Connection* connection=0, const Request_charsets *charsets=0) const {
		return cstr_to_string_body_taint(lang, connection, charsets).cstrm();
	}

	/// convert to constant C string with tainting dirty to lang
	const char* untaint_cstr(Language lang, SQL_Connection* connection=0, const Request_charsets *charsets=0) const {
		return cstr_to_string_body_untaint(lang, connection, charsets).cstr();
	}
	char *untaint_cstrm(Language lang, SQL_Connection* connection=0, const Request_charsets *charsets=0) const {
		return cstr_to_string_body_untaint(lang, connection, charsets).cstrm();
	}

	const char* untaint_and_transcode_cstr(Language lang, const Request_charsets *charsets) const;

	bool is_not_just_lang() const {
		return langs.opt.is_not_just_lang !=0;
	}
	
	Language just_lang() const {
		return langs.opt.lang;
	}

	char* visualize_langs() const;

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

	String& append_to(String& dest) const;
	String& append_to(String& dest, Language lang, bool forced=false) const;
	String& append(const String& src, Language lang, bool forced=false) { 
		return src.append_to(*this, lang, forced);
	}
	String& append_quoted(const String* src, Language lang=L_JSON){
		*this << "\"";
		if(src)
			this->append(*src, lang, true/*forced lang*/);
		*this << "\"";
		return *this;
	}

	String& operator << (const String& src) { return src.append_to(*this); }
	String& operator << (const char* src) { return append_help_length(src, 0, L_AS_IS); }
	String& operator << (const Body& src){
		langs.appendHelper(body, L_AS_IS, src);
		body<<src;
		return *this;
	}

	char first_char() const {
		return is_empty()?0:body.fetch(0);
	}

	char last_char() const {
		return is_empty()?0:body.fetch(body.length()-1);
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
	size_t pos(const Body substr, size_t this_offset=0, Language lang=L_UNSPECIFIED) const;
	/// String version of @see pos(const char*, int, Language)
	size_t pos(const String& substr, size_t this_offset=0, Language lang=L_UNSPECIFIED) const;
	size_t pos(char c, size_t this_offset=0) const {
		return body.pos(c, this_offset);
	}
	size_t pos(Charset& charset, const String& substr, size_t this_offset=0, Language lang=L_UNSPECIFIED) const;

	size_t strrpbrk(const char* chars, size_t left=0) const {
		return (length()) ? body.strrpbrk(chars, left, length()-1) : STRING_NOT_FOUND;
	}
	size_t strrpbrk(const char* chars, size_t left, size_t right) const {
		return body.strrpbrk(chars, left, right);
	}

	size_t rskipchars(const char* chars, size_t left=0) const {
		return (length()) ? body.rskipchars(chars, left, length()-1) : STRING_NOT_FOUND;
	}
	size_t rskipchars(const char* chars, size_t left, size_t right) const {
		return body.rskipchars(chars, left, right);
	}

	void split(ArrayString& result, size_t pos_after, const char* delim, Language lang=L_UNSPECIFIED) const;
	void split(ArrayString& result, size_t pos_after, const String& delim, Language lang=L_UNSPECIFIED) const;

	typedef void (*Row_action)(Table& table, ArrayString* row, int prestart, int prefinish, int poststart, int postfinish, void *info);

	/**
		@return table of found items, if any.
		table format is defined and fixed[can be used by others]: 
		@verbatim
			prematch/match/postmatch/1/2/3/...
		@endverbatim
	*/
	Table* match(VRegex* vregex, Row_action row_action, void *info, int& matches_count) const;

	enum Change_case_kind {
		CC_UPPER,
		CC_LOWER
	};
	String& change_case(Charset& source_charset, Change_case_kind kind) const;

	const String& replace(const Dictionary& dict) const;
	const String& trim(Trim_kind kind=TRIM_BOTH, const char* chars=0, Charset* source_charset=0) const;
	double as_double() const { return pa_atod(cstr(), this); }
	int as_int() const { return pa_atoi(cstr(), 0, this); }
	bool as_bool() const { return as_int()!=0; }
	const String& escape(Charset& source_charset) const;

private: //disabled

	String& operator = (const String&) { return *this; }

};

#ifndef HASH_CODE_CACHING
/// simple hash code of string. used by Hash
inline uint hash_code(const String::Body self) {
	return self.get_hash_code();
}
#endif


#endif
