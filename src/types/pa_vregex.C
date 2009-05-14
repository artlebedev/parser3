/** @file
	Parser: @b regex class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VREGEX_C="$Date: 2009/05/14 08:10:09 $";

#include "pa_vregex.h"
#include "pa_vint.h"


char* get_pcre_exec_error_text(int exec_result){
	switch(exec_result){
		case PCRE_ERROR_BADUTF8:
		case PCRE_ERROR_BADUTF8_OFFSET:
			return "UTF-8 validation failed during pcre_exec (%d).";
			break;
		default:
			return "execution error (%d)";
	}
}


Value& VRegex::as_expr_result(bool/*return_string_as_is=false*/) {
	return *new VInt(as_int());
}

void VRegex::regex_options(const String* options, int* result){
	struct Regex_option {
		const char* key;
		const char* keyAlt;
		int clear;
		int set;
		int *result;
	} regex_option[]={
		{"i", "I", 0, PCRE_CASELESS, result}, // a=A
		{"s", "S", 0, PCRE_DOTALL, result}, // ^\n\n$ [default]
		{"m", "M", PCRE_DOTALL, PCRE_MULTILINE, result}, // ^aaa\n$^bbb\n$
		{"x", 0, 0, PCRE_EXTENDED, result}, // whitespace in regex ignored
		{"U", 0, 0, PCRE_UNGREEDY, result}, // ungreedy patterns (greedy by default)
		{"g", "G", 0, MF_GLOBAL_SEARCH, result+1}, // many rows
		{"'", 0, 0, MF_NEED_PRE_POST_MATCH, result+1},
		{"n", 0, 0, MF_JUST_COUNT_MATCHES, result+1},
		{0, 0, 0, 0, 0}
	};
	result[0]=PCRE_EXTRA /* backslash+non-special char causes error */
			| PCRE_DOTALL /* dot matches all chars including newline char */
			| PCRE_DOLLAR_ENDONLY /* dollar matches only end of string, but not newline chars */;
	result[1]=0;

	if(options && !options->is_empty()) 
		for(Regex_option *o=regex_option; o->key; o++) 
			if(
				options->pos(o->key)!=STRING_NOT_FOUND
				|| (o->keyAlt && options->pos(o->keyAlt)!=STRING_NOT_FOUND)
			){
				*o->result &= ~o->clear;
				*o->result |= o->set;
			}
}


void VRegex::set(Charset& acharset, const String* aregex, const String* aoptions){
	if(aregex->is_empty())
		throw Exception(PARSER_RUNTIME,
			0,
			"regexp is empty");

	fcharset=&acharset;
	fpattern=aregex->cstr(String::L_UNSPECIFIED); // fix any tainted with L_REGEX

	regex_options(aoptions, foptions);
}


void VRegex::compile(){
	const char* err_ptr;
	int err_offset;
	int options=foptions[0];

	// @todo (for UTF-8): check string & pattern and use PCRE_NO_UTF8_CHECK option 
	if(fcharset->isUTF8())
		options|=PCRE_UTF8;

	fcode=pcre_compile(fpattern, options,
		&err_ptr, &err_offset,
		fcharset->pcre_tables);

	if(!fcode){
		throw Exception(PCRE_EXCEPTION_TYPE,
			new String(fpattern+err_offset, String::L_TAINTED),
			"regular expression syntax error - %s", err_ptr);
	}

}


size_t VRegex::full_info(int type){
	size_t result;
	int fullinfo_result=pcre_fullinfo(fcode, fextra, type, &result);
	if(fullinfo_result<0){
		throw Exception(PCRE_EXCEPTION_TYPE,
			new String(fpattern, String::L_TAINTED),
			"pcre_full_info error (%d)", fullinfo_result);
	}

	return result;
};


size_t VRegex::get_info_size(){
	return full_info(PCRE_INFO_SIZE);
}


size_t VRegex::get_study_size(){
	return full_info(PCRE_INFO_STUDYSIZE);
}

void VRegex::study(){
	if(fstudied)
		return;

	const char* err_ptr;
	fextra=pcre_study(fcode, 0/*options*/, &err_ptr);

	if(err_ptr){
		throw Exception(PCRE_EXCEPTION_TYPE,
			new String(fpattern, String::L_TAINTED),
			"pcre_study error: %s", err_ptr);
	}

	fstudied=true;
}


int VRegex::exec(const char* string, size_t string_len, int* ovector, int ovector_size, int prestart){
	int result=pcre_exec(fcode, fextra, 
		string, string_len, prestart,
		0, ovector, ovector_size);
			
	if(result<0 && result!=PCRE_ERROR_NOMATCH){
		throw Exception(PCRE_EXCEPTION_TYPE, 
			new String(fpattern, String::L_TAINTED),
			get_pcre_exec_error_text(result), result);
	}

	return result;
}


