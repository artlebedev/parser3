/** @file
	Parser: string class. @see untalength_t.C.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_STRING_C="$Date: 2004/04/02 13:57:07 $";

#include "pcre.h"

#include "pa_string.h"
#include "pa_exception.h"
#include "pa_table.h"
#include "pa_dictionary.h"
#include "pa_charset.h"

const String String::Empty;

int pa_atoi(const char* str, const String* problem_source) {
	if(!str)
		return 0;

	while(*str && isspace(*str))
		str++;
	if(!*str)
		return 0;

	int result;
	char *error_pos;
	// 0xABC
	if(str[0]=='0')
		if(str[1]=='x' || str[1]=='X')
			result=(int)(unsigned long)strtol(str, &error_pos, 0);
		else {
			 // skip leading 0000, to disable octal interpretation
			do str++; while(*str=='0');				
			result=(int)strtol(str, &error_pos, 0);
		}
	else
		result=(int)strtol(str, &error_pos, 0);

	while(char c=*error_pos++)
		if(!isspace(c))
			throw Exception("number.format",
				problem_source,
				problem_source?"invalid number (int)": "'%s' is invalid number (int)", str);

	return result;
}

double pa_atod(const char* str, const String* problem_source) {
	if(!str)
		return 0;

	while(*str && isspace(*str))
		str++;
	if(!*str)
		return 0;

	double result;
	char *error_pos;
	// 0xABC
	if(str[0]=='0')
		if(str[1]=='x' || str[1]=='X')
			result=(double)(unsigned long)strtol(str, &error_pos, 0);
		else
			result=(double)strtod(str+1/*skip leading 0*/, &error_pos);
	else
		result=(double)strtod(str, &error_pos);

	while(char c=*error_pos++)
		if(!isspace(c))
			throw Exception("number.format",
				problem_source,
				problem_source?"invalid number (double)": "'%s' is invalid number (double)", str);

	return result;
}

// cord lib extension

#ifndef DOXYGEN
typedef struct {
    ssize_t countdown;
    char target;	/* Character we're looking for	*/
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
	return String::Body(pa_strdup(local, length), length);
}

String::Body String::Body::trim(String::Trim_kind kind, const char* chars, 
								size_t* out_start, size_t* out_length) const {
	size_t our_length=length();
	if(!our_length)
		return *this;
	if(!chars)
		chars=" \t\n"; // white space
	Body result=*this;

	size_t start=0;
	size_t end=our_length;
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
};
uint String::Body::hash_code() const {
	uint result=0;
	CORD_iter5(body, 0,
		CORD_batched_iter_fn_generic_hash_code, 
		CORD_batched_iter_fn_generic_hash_code, &result);
	return result;
}

// String methods

String::String(const char* cstr, size_t helper_length, bool tainted): body(CORD_EMPTY) {
	append_help_length(cstr, helper_length, tainted?L_TAINTED:L_CLEAN);
}
String::String(const String::C cstr, bool tainted): body(CORD_EMPTY) {
	append_know_length(cstr.str, cstr.length, tainted?L_TAINTED:L_CLEAN);
}

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

//	SAPI::log("piece of '%s' from %d to %d is '%s'",
		//cstr(), substr_begin, substr_end, result.cstr());
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

size_t String::pos(const String& substr, 
				size_t this_offset, Language lang) const {
	return pos(substr.body, this_offset, lang);
}

void String::split(ArrayString& result, 
		   size_t& pos_after, 
		   const char* delim, 
		   Language lang, int limit) const {
	size_t self_length=length();
	if(size_t delim_length=strlen(delim)) {
		size_t pos_before;
		// while we have 'delim'...
		for(; (pos_before=pos(delim, pos_after, lang))!=STRING_NOT_FOUND && limit; limit--) {
			result+=&mid(pos_after, pos_before);
			pos_after=pos_before+delim_length;
		}
		// last piece
		if(pos_after<self_length && limit) {
			result+=&mid(pos_after, self_length);
			pos_after=self_length;
		}
	} else { // empty delim
		result+=this;
		pos_after+=self_length;
	}
}

void String::split(ArrayString& result, 
		   size_t& pos_after, 
		   const String& delim, Language lang, 
		   int limit) const {
	if(!delim.is_empty()) {
		size_t pos_before;
		// while we have 'delim'...
		for(; (pos_before=pos(delim, pos_after, lang))!=STRING_NOT_FOUND && limit; limit--) {
			result+=&mid(pos_after, pos_before);
			pos_after=pos_before+delim.length();
		}
		// last piece
		if(pos_after<length() && limit) {
			result+=&mid(pos_after, length());
			pos_after=length();
		}
	} else { // empty delim
		result+=this;
		pos_after+=length();
	}
}

static void regex_options(const String* options, int *result, bool& need_pre_post_match){
    struct Regex_option {
		const char* keyL;
		const char* keyU;
		int clear, set;
		int *result;
		bool *flag;
    } regex_option[]={
		{"i", "I", 0, PCRE_CASELESS, result, 0}, // a=A
		{"s", "S", 0, PCRE_DOTALL, result, 0}, // \n\n$ [default]
		{"x", "U", 0, PCRE_EXTENDED, result, 0}, // whitespace in regex ignored
		{"m", "M", PCRE_DOTALL, PCRE_MULTILINE, result, 0}, // ^aaa\n$^bbb\n$
		{"g", "G", 0, 1, result+1, 0}, // many rows
		{"'", 0, 0, 0, 0, &need_pre_post_match},
		{0, 0, 0, 0, 0, 0}
    };
	result[0]=PCRE_EXTRA | PCRE_DOTALL | PCRE_DOLLAR_ENDONLY;
	result[1]=0;

    if(options && !options->is_empty()) 
		for(Regex_option *o=regex_option; o->keyL; o++) 
			if(options->pos(o->keyL)!=STRING_NOT_FOUND
				|| (o->keyU && options->pos(o->keyU)!=STRING_NOT_FOUND)) {
				if(o->flag)
					*o->flag=true;
				else { // result
					*o->result &= ~o->clear;
					*o->result |= o->set;
				}
			}
}

Table* String::match(Charset& source_charset,
		     const String& regexp, 
		     const String* options,
		     Row_action row_action, void *info,
		     bool& just_matched) const { 
	if(regexp.is_empty())
		throw Exception(0,
			0,
			"regexp is empty");

	const char* pattern=regexp.cstr();
	const char* errptr;
	int erroffset;
	bool need_pre_post_match=false;
	int option_bits[2]={0};  regex_options(options, option_bits, need_pre_post_match);
	bool global=option_bits[1]!=0;
	pcre *code=pcre_compile(pattern, option_bits[0], 
		&errptr, &erroffset,
		source_charset.pcre_tables);

	if(!code)
		throw Exception(0,
			&regexp.mid(erroffset, regexp.length()),
			"regular expression syntax error - %s", errptr);
	
	int subpatterns=pcre_info(code, 0, 0);
	if(subpatterns<0) {
		pcre_free(code);
		throw Exception(0,
			&regexp,
			"pcre_info error (%d)", 
				subpatterns);
	}

	const char* subject=cstr();
	size_t subject_length=strlen(subject);
	const int oveclength=(1/*match*/+MAX_MATCH_GROUPS)*3;
	int ovector[oveclength];

	// create table
	Table::Action_options table_options;
	Table& table=*new Table(string_match_table_template, table_options);

	int exec_option_bits=0;
	int prestart=0;
	int poststart=0;
	int postfinish=length();
	while(true) {
		int exec_substrings=pcre_exec(code, 0,
			subject, subject_length, prestart,
			exec_option_bits, ovector, oveclength);
		
		if(exec_substrings==PCRE_ERROR_NOMATCH) {
			pcre_free(code);
			row_action(table, 0/*last time, no raw*/, 0, 0, poststart, postfinish, info);
			if(global || subpatterns)
				return &table; // global or with subpatterns=true+result
			else {
				just_matched=false; return 0; // not global=no result
			}
		}

		if(exec_substrings<0) {
			pcre_free(code);
			throw Exception(0,
				&regexp,
				"regular expression execute error (%d)", 
					exec_substrings);
		}

		int prefinish=ovector[0];
		poststart=ovector[1];
		ArrayString* row=new ArrayString;
		if(need_pre_post_match) {
			*row+=&mid(0, prefinish); // .prematch column value
			*row+=&mid(prefinish, poststart); // .match
			*row+=&mid(poststart, postfinish); // .postmatch
		} else {
			*row+=&Empty; // .prematch column value
			*row+=&Empty; // .match
			*row+=&Empty; // .postmatch
		}
		
		for(int i=1; i<exec_substrings; i++) {
			// -1:-1 case handled peacefully by mid() itself
			*row+=&mid(ovector[i*2+0], ovector[i*2+1]); // .i column value
		}
		
		row_action(table, row, prestart, prefinish, poststart, postfinish, info);

		if(!global || prestart==poststart) { // not global | going to hang
			pcre_free(code);
			row_action(table, 0/*last time, no row*/, 0, 0, poststart, postfinish, info);
			return &table;
		}
		prestart=poststart;

/*
		if(option_bits[0] & PCRE_MULTILINE)
			exec_option_bits|=PCRE_NOTBOL; // start of subject+startoffset not BOL
*/
	}
}

String& String::change_case(Charset& source_charset, Change_case_kind kind) const {
	String& result=*new String();
	if(is_empty())
		return result;

	char* new_cstr=cstrm();
	size_t new_cstr_len=length();
	if(source_charset.isUTF8()) {
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

const String& String::replace(const Dictionary& dict) const {
	String& result=*new String();
	const char* old_cstr=cstr();
	const char* prematch_begin=old_cstr;

	const char* current=old_cstr;
	while(*current) {
		if(Dictionary::Subst subst=dict.first_that_begins(current)) {
			// prematch
			if(size_t prematch_length=current-prematch_begin) {
				result.langs.append(result.body, langs, prematch_begin-old_cstr, prematch_length);
				result.body.append_strdup_know_length(prematch_begin, prematch_length);
			}

			// match
			// skip 'a' in 'current'; move prematch_begin
			current+=subst.from_length; prematch_begin=current;

			if(const String* b=subst.to) // are there any b?
				result<<*b;
		} else // simply advance
			current++; 
	}

	// postmatch
	if(size_t postmatch_length=current-prematch_begin) {
		result.langs.append(result.body, langs, prematch_begin-old_cstr, postmatch_length);
		result.body.append_strdup_know_length(prematch_begin, postmatch_length);
	}

	ASSERT_STRING_INVARIANT(result);
	return result;
}

static int serialize_body_char(char c, char** cur) {
	*((*cur)++)=c;
	return 0; // 0=continue
};
static int serialize_body_piece(const char* s, char** cur) {
	size_t length=strlen(s);
	memcpy(*cur, s, length);  *cur+=length;
	return 0; // 0=continue
};
static int serialize_lang_piece(char alang, size_t asize, char** cur) {
	// lang
	**cur=alang; (*cur)++;
	// length [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(*cur, &asize, sizeof(asize));  *cur+=sizeof(asize);

	return 0; // 0=continue
}
String::Cm String::serialize(size_t prolog_length) const {
	size_t fragments_count=langs.count();
	size_t buf_length=
		prolog_length //1
		+sizeof(size_t) //2
		+fragments_count*(sizeof(char)+sizeof(size_t)) //3
		+body.length() //4
		+1; // for zero terminator used in deserialize
	String::Cm result(new(PointerFreeGC) char[buf_length], buf_length);

	// 1: prolog
	char *cur=result.str+prolog_length;
	// 2: langs.count [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(cur, &fragments_count, sizeof(fragments_count));  cur+=sizeof(fragments_count);
	// 3: lang info
	langs.for_each(body, serialize_lang_piece, &cur);
	// 4: letters
	body.for_each(serialize_body_char, serialize_body_piece, &cur);
	// 5: zero terminator
	*cur=0;

	return result;
}
bool String::deserialize(size_t prolog_length, void *buf, size_t buf_length) {
	if(buf_length<=prolog_length)
		return false;
	buf_length-=prolog_length;
	buf_length-=1; // 5: zero terminator

	// 1: prolog
	const char* cur=(const char* )buf+prolog_length;

	// 2: langs.count
	size_t fragments_count;
	if(buf_length<sizeof(fragments_count)) // langs.count don't fit?
		return false;
	// [WARNING: not cast, addresses must be %4=0 on sparc]
	memcpy(&fragments_count, cur, sizeof(fragments_count));  cur+=sizeof(fragments_count);
	buf_length-=sizeof(fragments_count);
	
	if(fragments_count) {
		// 3: lang info
		size_t total_length=0;
		for(size_t f=0; f<fragments_count; f++) {
			char lang;
			size_t fragment_length;
			size_t piece_length=sizeof(lang)+sizeof(fragment_length);
			if(buf_length<piece_length) // lang+length
				return false;

			// lang
			lang=*cur++;
			// length [WARNING: not cast, addresses must be %4=0 on sparc]
			memcpy(&fragment_length, cur, sizeof(fragment_length));  cur+=sizeof(fragment_length);

			// uchar needed to prevent propagating 0x80 bit to upper bytes
			langs.append(total_length, (String::Language)(uchar)lang, fragment_length);
			total_length+=fragment_length;

			buf_length-=piece_length;
		}

		// 4: letters
		if(buf_length!=total_length)
			return false;

		// serialize wrote extra zero byte there, we can rely on that
		body=String::Body(cur, buf_length);
	}

	ASSERT_STRING_INVARIANT(*this);
	return true;
}

const char* String::Body::v() const {
	return CORD_to_const_char_star(body);
}
const char* String::Languages::v() const {
	if(opt.is_not_just_lang)
		return CORD_to_const_char_star(langs);
	else
		return (const char*)&langs;
}
const char* String::v() const {
	const int LIMIT_VIEW=20;
	char* buf=(char*)malloc(MAX_STRING);
	const char*body_view=body.v();
	const char*langs_view=langs.v();
	snprintf(buf, MAX_STRING, 
		"%d:%.*s%s}   "
		"{%d:%s",
		langs.count(), LIMIT_VIEW, langs_view, strlen(langs_view)>LIMIT_VIEW?"...":"",
		strlen(body_view), body_view
	);

	return buf;
}

const String& String::trim(String::Trim_kind kind, const char* chars) const {
	if(!length())
		return *this;

	size_t substr_begin, substr_length;
	Body new_body=body.trim(kind, chars, &substr_begin, &substr_length);
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