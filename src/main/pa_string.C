/** @file
	Parser: string class. @see untalength_t.C.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_string.h"
#include "pa_exception.h"
#include "pa_table.h"
#include "pa_dictionary.h"
#include "pa_charset.h"
#include "pa_vregex.h"

volatile const char * IDENT_PA_STRING_C="$Id: pa_string.C,v 1.266 2020/12/15 17:10:37 moko Exp $" IDENT_PA_STRING_H;

const String String::Empty;

#define COMPILE_ASSERT(x) extern int assert_checker[(x) ? 1 : -1]
COMPILE_ASSERT(sizeof(String::Languages) == sizeof(CORD));

// pa_atoui is based on Manuel Novoa III _strto_l for uClibc

template<typename T> inline T pa_ato_any(const char *str, int base, const String* problem_source,const T max){
	T result = 0;
	const char *pos = str;

	while (isspace(*pos)) /* skip leading whitespace */
		++pos;

	if (base == 16 && *pos == '0') { /* handle option prefix */
		++pos;
		if (*pos == 'x' || *pos == 'X') {
			++pos;
		}
	}

	if (base == 0) { /* dynamic base */
		base = 10; /* default is 10 */
		if (*pos == '0') {
			++pos;
			if (*pos == 'x' || *pos == 'X'){
				++pos;
				base=16;
			}
		}
	}

	if (base < 2 || base > 16) { /* illegal base */
		throw Exception(PARSER_RUNTIME, 0, "base to must be an integer from 2 to 16");
	}

	T cutoff = max / base;
	int cutoff_digit = (int)(max - cutoff * base);

	while(true) {
		int digit;
		
		if ((*pos >= '0') && (*pos <= '9')) {
			digit = (*pos - '0');
		} else if (*pos >= 'a') {
			digit = (*pos - 'a' + 10);
		} else if (*pos >= 'A') {
			digit = (*pos - 'A' + 10);
		} else break;

		if (digit >= base) {
			break;
		}
		
		++pos;
		
		/* adjust number, with overflow check */
		if ((result > cutoff) || ((result == cutoff) && (digit > cutoff_digit))) {
			throw Exception("number.format", problem_source, problem_source ? "out of range (int)" : "'%s' is out of range (int)", str);
		} else {
			result  = result * base + digit;
		}
	}

	while(char c=*pos++)
		if(!isspace(c))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (int)" : "'%s' is invalid number (int)", str);

	return result;
}

unsigned int pa_atoui(const char *str, int base, const String* problem_source){
	if(!str)
		return 0;

	return pa_ato_any<unsigned int>(str, base, problem_source, UINT_MAX);
}

uint64_t pa_atoul(const char *str, int base, const String* problem_source){
	if(!str)
		return 0;

	return pa_ato_any<uint64_t>(str, base, problem_source, ULLONG_MAX);
}

int pa_atoi(const char* str, int base, const String* problem_source) {
	if(!str)
		return 0;

	while(isspace(*str))
		str++;

	if(!*str)
		return 0;

	bool negative=false;
	if(str[0]=='-') {
		negative=true;
		str++;
	} else if(str[0]=='+') {
		str++;
	}

	unsigned int result=pa_atoui(str, base, problem_source);

	if(negative && result <= ((unsigned int)(-(1+INT_MIN)))+1)
		return -(int)result;
	
	if(result<=INT_MAX)
		return (int)result;
	
	throw Exception("number.format", problem_source, problem_source ? "out of range (int)" : "'%s' is out of range (int)", str);
}

double pa_atod(const char* str, const String* problem_source) {
	if(!str)
		return 0;

	while(isspace(*str))
		str++;

	if(!*str)
		return 0;

	bool negative=false;
	if(str[0]=='-') {
		negative=true;
		str++;
	} else if(str[0]=='+') {
		str++;
	}

	double result;
	if(str[0]=='0') {
		if(str[1]=='x' || str[1]=='X') {
			// 0xABC
			result=(double)pa_atoul(str, 0, problem_source);
			return negative ? -result : result;
		} else {
			 // skip leading 0000, to disable octal interpretation
			do str++; while(*str=='0');
		}
	}

	char *error_pos;
	result=strtod(str, &error_pos);

	while(char c=*error_pos++)
		if(!isspace((unsigned char)c))
			throw Exception("number.format", problem_source, problem_source ? "invalid number (double)" : "'%s' is invalid number (double)", str);

	return negative ? -result : result;
}

// cord lib extension

#ifndef DOXYGEN
typedef struct {
	ssize_t countdown;
	int target;	/* Character we're looking for	*/
} chr_data;
#endif

static int CORD_range_contains_chr_greater_then_proc(char c, size_t size, void* client_data)
{
	register chr_data * d = (chr_data *)client_data;

	if (d -> countdown<=0) return(2);
	d -> countdown -= size;
	if (c > d -> target) return(1);
	return(0);
}

int CORD_range_contains_chr_greater_then(CORD x, size_t i, size_t n, int c)
{
	chr_data d;

	d.countdown = n;
	d.target = c;
	return(CORD_block_iter(x, i, CORD_range_contains_chr_greater_then_proc, &d) == 1/*alternatives: 0 normally ended, 2=struck 'n'*/);
}

static int CORD_block_count_proc(char /*c*/, size_t /*size*/, void* client_data)
{
	int* result=(int*)client_data;
	(*result)++;
	return(0); // 0=continue
}

size_t CORD_block_count(CORD x)
{
	size_t result=0;
	CORD_block_iter(x, 0, CORD_block_count_proc, &result);
	return result;
}

// helpers

/// String::match uses this as replace & global search table columns

const int MAX_MATCH_GROUPS=100;

class String_match_table_template_columns: public ArrayString {
public:
	String_match_table_template_columns() {
		*this+=new String("prematch");
		*this+=new String("match");
		*this+=new String("postmatch");
		for(int i=0; i<MAX_MATCH_GROUPS; i++) {
			*this+=new String(String::Body::Format(1+i), String::L_CLEAN);
		}
	}
};

Table string_match_table_template(new String_match_table_template_columns);

// String::Body methods

String::Body String::Body::Format(int value) {
	char local[MAX_NUMBER];
	size_t length=snprintf(local, MAX_NUMBER, "%d", value);
	return String::Body(pa_strdup(local, length));
}

String::Body String::Body::trim(String::Trim_kind kind, const char* chars, size_t* out_start, size_t* out_length, Charset* source_charset) const {
	size_t our_length=length();
	if(!our_length)
		return *this;

	// check if any UTF-8 in chars
	bool fast=true;
	if(chars && source_charset && source_charset->isUTF8()){
		const char* pos=chars;
		while(unsigned char c=*pos++)
			if(c>127){
				fast=false;
				break;
			}
	}

	size_t start=0;
	size_t end=our_length;
	if(!chars)
		chars=" \t\n"; // white space

	if(fast){
		// from left...
		if(kind!=TRIM_END) {
			CORD_pos pos; set_pos(pos, 0);
			while(true) {
				char c=CORD_pos_fetch(pos);
				if(strchr(chars, c)) {
					if(++start==our_length)
						return 0; // all chars are empty, just return empty string
				} else
					break;			

				CORD_next(pos);
			}
		}

		// from right..
		if(kind!=TRIM_START) {
			CORD_pos pos; set_pos(pos, end-1);
			while(true) {
				char c=CORD_pos_fetch(pos);
				if(strchr(chars, c)) {
					if(--end==0) // optimization: NO need to check for 'end>=start', that's(<) impossible
						return 0; // all chars are empty, just return empty string
				} else
					break;			

				CORD_prev(pos);
			}
		}
	} else {
		const XMLByte* src_begin=(const XMLByte*)cstr();
		const XMLByte* src_end=src_begin+our_length;

		// from left...
		if(kind!=TRIM_END) {
			while(src_begin<src_end){
				uint char_length=1;
				const XMLByte* ptr=src_begin;
				// searching first UTF-8 byte: http://tools.ietf.org/html/rfc3629#section-3
				while(++src_begin<=src_end && (*src_begin>127 && *src_begin<0xC0))
					char_length++;

				bool found=false;
				for(const char* chars_byte=chars; chars_byte=strchr(chars_byte, *ptr); chars_byte++)
					if(strncmp(chars_byte, (const char*)ptr, char_length)==0){
						found=true;
						break;
					}

				if(found){
					start+=char_length;
					if(start==our_length)
						return 0; // all chars are empty, just return empty string
				} else
					break;
			}
		}

		// from right..
		if(kind!=TRIM_START) {
			while(src_begin<src_end){
				uint char_length=1;
				// searching first UTF-8 byte: http://tools.ietf.org/html/rfc3629#section-3
				while(src_begin<=--src_end && (*src_end>127 && *src_end<0xC0))
					char_length++;

				bool found=false;
				for(const char* chars_byte=chars; chars_byte=strchr(chars_byte, *src_end); chars_byte++)
					if(strncmp(chars_byte, (const char*)src_end, char_length)==0){
						found=true;
						break;
					}

				if(found){
					end-=char_length;
					if(end==0)
						return 0; // all chars are empty, just return empty string
				} else
					break;
			}
		}
	}

	if(start==0 && end==our_length) // nobody moved a thing
		return *this;

	if(out_start)
		*out_start=start;
	size_t new_length=end-start;
	if(out_length)
		*out_length=new_length;

	return mid(start, new_length);
}

static int CORD_batched_iter_fn_generic_hash_code(char c, void * client_data) {
	uint& result=*static_cast<uint*>(client_data);
	generic_hash_code(result, c);
	return 0;
}

static int CORD_batched_iter_fn_generic_hash_code(const char*  s, void * client_data) {
	uint& result=*static_cast<uint*>(client_data);
	generic_hash_code(result, s);
	return 0;
}

uint String::Body::get_hash_code() const {
#ifdef HASH_CODE_CACHING
	if(hash_code)
		return hash_code;
#else
	uint hash_code=0;
#endif
	if (body && CORD_IS_STRING(body)){
		generic_hash_code(hash_code, (const char *)body);
	} else {
		CORD_iter5(body, 0,
			CORD_batched_iter_fn_generic_hash_code, 
			CORD_batched_iter_fn_generic_hash_code, &hash_code);
	}
	return hash_code;
}

struct CORD_pos_info {
	const char* chars;
	size_t left;
	size_t pos;
};

// can be called only for IS_FUNCTION(CORD) which is used in String::Body::strrpbrk
static int CORD_iter_fn_rpos(char c, CORD_pos_info* info) {
	if(info->pos < info->left){
		info->pos=STRING_NOT_FOUND;
		return 1;
	}
	if(strchr(info->chars, c))
		return 1;
	--(info->pos);
	return 0;
}

size_t String::Body::strrpbrk(const char* chars, size_t left, size_t right) const {
	if(is_empty() || !chars || !strlen(chars))
		return STRING_NOT_FOUND;
	CORD_pos_info info={chars, left, right};
	if(CORD_riter4(body, right, (CORD_iter_fn)CORD_iter_fn_rpos, &info))
		return info.pos;
	else
		return STRING_NOT_FOUND;
}


// can be called only for IS_FUNCTION(CORD) which is used in String::Body::rskipchars
static int CORD_iter_fn_rskip(char c, CORD_pos_info* info) {
	if(info->pos < info->left) {
		info->pos=STRING_NOT_FOUND;
		return 1;
	}
	if(!strchr(info->chars, c))
		return 1;
	--(info->pos);
	return 0;
}

size_t String::Body::rskipchars(const char* chars, size_t left, size_t right) const {
	if(is_empty() || !chars || !strlen(chars))
		return STRING_NOT_FOUND;
	CORD_pos_info info={chars, left, right};
	if(CORD_riter4(body, right, (CORD_iter_fn)CORD_iter_fn_rskip, &info))
		return info.pos;
	else
		return STRING_NOT_FOUND;
}

// String methods

String& String::append_know_length(const char* str, size_t known_length, Language lang) {
	if(!known_length)
		return *this;

	// first: langs
	langs.append(body, lang, known_length);
	// next: letters themselves
	body.append_know_length(str, known_length);

	ASSERT_STRING_INVARIANT(*this);
	return *this;
}

String& String::append_help_length(const char* str, size_t helper_length, Language lang) {
	if(!str)
		return *this;
	size_t known_length=helper_length?helper_length:strlen(str);
	if(!known_length)
		return *this;

	return append_know_length(str, known_length, lang);
}

String::String(int value, const char *format) : langs(L_CLEAN){
	char buf[MAX_NUMBER];
	body.append_strdup_know_length(buf, snprintf(buf, MAX_NUMBER, format, value));
}

String& String::append_strdup(const char* str, size_t helper_length, Language lang) {
	size_t known_length=helper_length?helper_length:strlen(str);
	if(!known_length)
		return *this;

	// first: langs
	langs.append(body, lang, known_length);
	// next: letters themselves
	body.append_strdup_know_length(str, known_length);

	ASSERT_STRING_INVARIANT(*this);
	return *this;
}

struct CORD_length_info {
	size_t len;
	size_t skip;
};

int CORD_batched_len(const char* s, CORD_length_info* info){
	info->len += lengthUTF8( (const XMLByte *)s, (const XMLByte *)s+strlen(s));
	return 0;
}

// can be called only for IS_FUNCTION(CORD) which are used in large String::Body::mid
int CORD_batched_len(const char c, CORD_length_info* info){
	if (info->skip==0){
		info->len++;
		info->skip = lengthUTF8Char(c)-1;
	} else {
		info->skip--;
	}
	return 0;
}

size_t String::length(Charset& charset) const {
	if(charset.isUTF8()){
		CORD_length_info info = {0, 0};
		body.for_each<CORD_length_info *>(CORD_batched_len, CORD_batched_len, &info);
		return info.len;
	} else
		return body.length();
}

/// @todo check in doc: whether it documents NOW bad situation "abc".mid(-1, 3) =were?="ab"
String& String::mid(size_t substr_begin, size_t substr_end) const {
	String& result=*new String;

	size_t self_length=length();
	substr_begin=min(substr_begin, self_length);
	substr_end=min(max(substr_end, substr_begin), self_length);
	size_t substr_length=substr_end-substr_begin;
	if(!substr_length)
		return result;

	// first: their langs
	result.langs.append(result.body, langs, substr_begin, substr_length);
	// next: letters themselves
	result.body=body.mid(substr_begin, substr_length);

	ASSERT_STRING_INVARIANT(result);
	return result;
}

// from, to and helper_length in characters, not in bytes (it's important for utf-8)
String& String::mid(Charset& charset, size_t from, size_t to, size_t helper_length) const {
	String& result=*new String;

	size_t self_length=helper_length ? helper_length : length(charset);

	if(!self_length)
		return result;

	from=min(min(to, from), self_length);
	to=min(max(to, from), self_length);

	size_t substr_length=to-from;

	if(!substr_length)
		return result;

	if(charset.isUTF8()){
		const XMLByte* src_begin=(const XMLByte*)cstr();
		const XMLByte* src_end=src_begin+body.length();

		// convert 'from' and 'substr_length' from 'characters' to 'bytes'
		from=getUTF8BytePos(src_begin, src_end, from);
		substr_length=getUTF8BytePos(src_begin+from, src_end, substr_length);
		if(!substr_length)
			return result;
	}

	// first: their langs
	result.langs.append(result.body, langs, from, substr_length);
	// next: letters themselves
	result.body=body.mid(from, substr_length);

	ASSERT_STRING_INVARIANT(result);
	return result;
}

size_t String::pos(const String::Body substr, size_t this_offset, Language lang) const {
	size_t substr_length=substr.length();
	while(true) {
		size_t substr_begin=body.pos(substr, this_offset);
		
		if(substr_begin==CORD_NOT_FOUND)
			return STRING_NOT_FOUND;

		if(langs.check_lang(lang, substr_begin, substr_length))
			return substr_begin;

		this_offset=substr_begin+substr_length;
	}
}

size_t String::pos(const String& substr, size_t this_offset, Language lang) const {
	return pos(substr.body, this_offset, lang);
}

size_t String::pos(Charset& charset, const String& substr, size_t this_offset, Language lang) const {

	if(charset.isUTF8()){
		const XMLByte* srcPtr=(const XMLByte*)cstr();
		const XMLByte* srcEnd=srcPtr+body.length();

		// convert 'this_offset' from 'characters' to 'bytes'
		this_offset=getUTF8BytePos(srcPtr, srcEnd, this_offset);

		size_t result=pos(substr.body, this_offset, lang);
		return (result==CORD_NOT_FOUND)
			? STRING_NOT_FOUND
			: getUTF8CharPos(srcPtr, srcEnd, result); // convert 'result' from 'bytes' to 'characters'
	} else {
		size_t result=pos(substr.body, this_offset, lang);
		return (result==CORD_NOT_FOUND)
			? STRING_NOT_FOUND
			: result;
	}
}

void String::split(ArrayString& result, size_t pos_after, const char* delim, Language lang) const {
	if(is_empty())
		return;
	size_t self_length=length();
	if(size_t delim_length=strlen(delim)) {
		size_t pos_before;
		// while we have 'delim'...
		while((pos_before=pos(delim, pos_after, lang))!=STRING_NOT_FOUND) {
			result+=&mid(pos_after, pos_before);
			pos_after=pos_before+delim_length;
		}
		// last piece
		if(pos_after<self_length)
			result+=&mid(pos_after, self_length);
	} else { // empty delim
		result+=this;
	}
}

void String::split(ArrayString& result, size_t pos_after, const String& delim, Language lang) const {
	if(is_empty())
		return;
	if(!delim.is_empty()) {
		size_t pos_before;
		// while we have 'delim'...
		while((pos_before=pos(delim, pos_after, lang))!=STRING_NOT_FOUND) {
			result+=&mid(pos_after, pos_before);
			pos_after=pos_before+delim.length();
		}
		// last piece
		if(pos_after<length())
			result+=&mid(pos_after, length());
	} else { // empty delim
		result+=this;
	}
}

Table* String::match(VRegex* vregex, Row_action row_action, void *info, int& matches_count) const {

	// vregex->info(); // I have no idea what does it for?

	bool need_pre_post_match=vregex->is_pre_post_match_needed();
	bool global=vregex->is_global_search();

	const char* subject=cstr();
	size_t subject_length=length();
	const int ovector_size=(1/*match*/+MAX_MATCH_GROUPS)*3;
	int ovector[ovector_size];

	Table::Action_options table_options;
	Table& table=*new Table(string_match_table_template, table_options);

	int prestart=0;
	int poststart=0;
	int postfinish=length();
	while(true) {
		int exec_result=vregex->exec(subject, subject_length, ovector, ovector_size, prestart);

		if(exec_result<0) // only PCRE_ERROR_NOMATCH might be here, other negative results cause an exception
			break;

		int prefinish=ovector[0];
		poststart=ovector[1];

		if (prestart==poststart && subject[poststart]=='\n'){
			prestart++;
			continue;
		}

		ArrayString* row=new ArrayString(3);
		if(need_pre_post_match) {
			*row+=&mid(0, prefinish); // .prematch column value
			*row+=&mid(prefinish, poststart); // .match
			*row+=&mid(poststart, postfinish); // .postmatch
		} else {
			*row+=&Empty; // .prematch column value
			*row+=&Empty; // .match
			*row+=&Empty; // .postmatch
		}
		
		for(int i=1; i<exec_result; i++) {
			// -1:-1 case handled peacefully by mid() itself
			*row+=(ovector[i*2+0]>=0 && ovector[i*2+1]>0)?&mid(ovector[i*2+0], ovector[i*2+1]):new String; // .i column value
		}
		
		matches_count++;
		row_action(table, row, prestart, prefinish, poststart, postfinish, info);

		if(!global || prestart==poststart) // last step
			break;

		prestart=poststart;
	}

	row_action(table, 0/*last time, no raw*/, 0, 0, poststart, postfinish, info);
	return vregex->is_just_count() ? 0 : &table;
}

String& String::change_case(Charset& source_charset, Change_case_kind kind) const {
	String& result=*new String();
	if(is_empty())
		return result;

	char* new_cstr=cstrm();

	if(source_charset.isUTF8()) {
		size_t new_cstr_len=length();
		switch(kind) {
		case CC_UPPER:
			change_case_UTF8((const XMLByte*)new_cstr, new_cstr_len, (XMLByte*)new_cstr, new_cstr_len, UTF8CaseToUpper);
			break;
		case CC_LOWER:
			change_case_UTF8((const XMLByte*)new_cstr, new_cstr_len, (XMLByte*)new_cstr, new_cstr_len, UTF8CaseToLower);
			break;
		default:
			assert(!"unknown change case kind");
			break; // never
		}	
		
	} else {
		const unsigned char *tables=source_charset.pcre_tables;

		const unsigned char *a;
		const unsigned char *b;
		switch(kind) {
		case CC_UPPER:
			a=tables+lcc_offset;
			b=tables+fcc_offset;
			break;
		case CC_LOWER:
			a=tables+lcc_offset;
			b=0;
			break;
		default:
			assert(!"unknown change case kind");
			a=b=0; // calm, compiler
			break; // never
		}	

		char *dest=new_cstr;
		unsigned char index;
		for(const char* current=new_cstr; (index=(unsigned char)*current); current++) {
			unsigned char c=a[index];
			if(b)
				c=b[c];

			*dest++=(char)c;
		}
	}
	result.langs=langs;
	result.body=new_cstr;

	return result;
}

const String& String::escape(Charset& source_charset) const {
	if(is_empty())
		return *this;

	return Charset::escape(*this, source_charset);
}

#define STRING_APPEND(result, from_cstr, langs, langs_offset, length) \
			result.langs.append(result.body, langs, langs_offset, length); \
			result.body.append_strdup_know_length(from_cstr, length);

const String& String::replace(const Dictionary& dict) const {
	if(!dict.count() || is_empty())
		return *this;

	String& result=*new String();
	const char* old_cstr=cstr();
	const char* prematch_begin=old_cstr;

	if(dict.count()==1) {
		// optimized simple case

		Dictionary::Subst subst=dict.get(0);
		while(const char* p=strstr(prematch_begin, subst.from)) {
			// prematch
			if(size_t prematch_length=p-prematch_begin) {
				STRING_APPEND(result, prematch_begin, langs, prematch_begin-old_cstr, prematch_length)
			}

			// match
			prematch_begin=p+subst.from_length;

			if(const String* b=subst.to) // are there any b?
				result<<*b;
		}

	} else {

		const char* current=old_cstr;
		while(*current) {
			if(Dictionary::Subst subst=dict.first_that_begins(current)) {
				// prematch
				if(size_t prematch_length=current-prematch_begin) {
					STRING_APPEND(result, prematch_begin, langs, prematch_begin-old_cstr, prematch_length)
				}

				// match
				// skip 'a' in 'current'; move prematch_begin
				current+=subst.from_length; prematch_begin=current;

				if(const String* b=subst.to) // are there any b?
					result<<*b;
			} else // simply advance
				current++; 
		}

	}

	if(prematch_begin==old_cstr) // not modified
		return *this;

	// postmatch
	if(size_t postmatch_length=old_cstr+length()-prematch_begin) {
		STRING_APPEND(result, prematch_begin, langs, prematch_begin-old_cstr, postmatch_length)
	}

	ASSERT_STRING_INVARIANT(result);
	return result;
}

static int serialize_body_char(char c, char** cur) {
	*((*cur)++)=c;
	return 0; // 0=continue
}

static int serialize_body_piece(const char* s, char** cur) {
	size_t length=strlen(s);
	memcpy(*cur, s, length);  *cur+=length;
	return 0; // 0=continue
}

static int serialize_lang_piece(char alang, size_t asize, char** cur) {
	// lang
	**cur=alang; (*cur)++;
	// length [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(*cur, &asize, sizeof(asize));  *cur+=sizeof(asize);

	return 0; // 0=continue
}

String::Cm String::serialize(size_t prolog_length) const {
	size_t fragments_count=langs.count();
	size_t body_length=body.length();
	size_t buf_length=
		prolog_length //1
		+sizeof(size_t) //2
		+body_length //3
		+1 // 4 for zero terminator used in deserialize
		+sizeof(size_t) //5
		+fragments_count*(sizeof(char)+sizeof(size_t)); //6

	String::Cm result(new(PointerFreeGC) char[buf_length], buf_length);

	// 1: prolog
	char *cur=result.str+prolog_length;
	// 2: chars.count [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(cur, &body_length, sizeof(body_length));  cur+=sizeof(body_length);
	// 3: letters
	body.for_each(serialize_body_char, serialize_body_piece, &cur);
	// 4: zero terminator
	*cur++=0;
	// 5: langs.count [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(cur, &fragments_count, sizeof(fragments_count));  cur+=sizeof(fragments_count);
	// 6: lang info
	langs.for_each(body, serialize_lang_piece, &cur);

	return result;
}

bool String::deserialize(size_t prolog_size, void *buf, size_t buf_size) {
	size_t in_buf=buf_size;
	if(in_buf<=prolog_size)
		return false;
	in_buf-=prolog_size;

	// 1: prolog
	const char* cur=(const char* )buf+prolog_size;

	// 2: chars.count
	size_t body_length;
	if(in_buf<sizeof(body_length)) // body.length don't fit?
		return false;
	// [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(&body_length, cur, sizeof(body_length));  cur+=sizeof(body_length);
	in_buf-=sizeof(body_length);

	if(in_buf<body_length+1) // letters+terminator don't fit?
		return false;
	// 4: zero terminator
	if(cur[body_length] != 0) // in place?
		return false;
	// 3: letters
	body=String::Body(String::C(cur, body_length));
	cur+=body_length+1;
	in_buf-=body_length+1;

	// 5: langs.count
	size_t fragments_count;
	if(in_buf<sizeof(fragments_count)) // langs.count don't fit?
		return false;
	// [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(&fragments_count, cur, sizeof(fragments_count));  cur+=sizeof(fragments_count);
	in_buf-=sizeof(fragments_count);
	
	if(fragments_count) {
		// 6: lang info
		size_t total_length=0;
		for(size_t f=0; f<fragments_count; f++) {
			char lang;
			size_t fragment_length;
			size_t piece_length=sizeof(lang)+sizeof(fragment_length);
			if(in_buf<piece_length) // lang+length
				return false;

			// lang
			lang=*cur++;
			// length [WARNING: not cast, addresses must be %4=0 on sparc]
			memcpy(&fragment_length, cur, sizeof(fragment_length));  cur+=sizeof(fragment_length);

			size_t combined_length=total_length+fragment_length;
			if(combined_length>body_length)
				return false; // file curruption
			// uchar needed to prevent propagating 0x80 bit to upper bytes
			langs.append(total_length, (String::Language)(uchar)lang, fragment_length);
			total_length=combined_length;
			in_buf-=piece_length;
		}

		if(total_length!=body_length) // length(all language fragments) vs length(letters)
			return false;
	}
	if(in_buf!=0) // some strange extra bytes
		return false;

	ASSERT_STRING_INVARIANT(*this);
	return true;
}

void String::Body::dump() const {
	CORD_dump(body);
}

const char* String::Languages::visualize() const {
	if(opt.is_not_just_lang)
		return CORD_to_const_char_star(langs, 0);
	else
		return 0;
}

void String::Languages::dump() const {
	if(opt.is_not_just_lang)
		CORD_dump(langs);
	else
		puts((const char*)&langs);
}

void String::dump() const {
	body.dump();
	langs.dump();
}

static char *n_chars(char c, size_t length){
	char *result=(char *)pa_malloc_atomic(length+1);
	memset(result, c, length);
	result[length] = '\0';
	return result;
}

char* String::visualize_langs() const {
	return is_not_just_lang() ? pa_strdup(langs.visualize()) : n_chars((char)just_lang(), length());
}

const String& String::trim(String::Trim_kind kind, const char* chars, Charset* source_charset) const {
	if(is_empty())
		return *this;

	size_t substr_begin, substr_length;
	Body new_body=body.trim(kind, chars, &substr_begin, &substr_length, source_charset);
	if(new_body==body) // we received unchanged pointer, do likewise
		return *this;
	// new_body differs from body, adjust langs along

	String& result=*new String;
	if(!new_body) // body.trim produced empty result
		return result;
	// body.trim produced nonempty result

	// first: their langs
	result.langs.append(result.body, langs, substr_begin, substr_length);
	// next: letters themselves
	result.body=new_body;

	ASSERT_STRING_INVARIANT(result);
	return result;
}
