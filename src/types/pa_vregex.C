/** @file
	Parser: @b regex class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vregex.h"
#include "pa_vint.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_VREGEX_C="$Id: pa_vregex.C,v 1.21 2020/12/15 17:10:42 moko Exp $" IDENT_PA_VREGEX_H;

// defines

#define REGEX_PATTERN_NAME "pattern"
#define REGEX_OPTIONS_NAME "options"


const char* get_pcre_exec_error_text(int exec_result){
	switch(exec_result){
		case PCRE_ERROR_BADUTF8:
		case PCRE_ERROR_BADUTF8_OFFSET:
			return "UTF-8 validation failed during pcre_exec (%d).";
			break;
		default:
			return "execution error (%d)";
	}
}


Value& VRegex::as_expr_result() {
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

	if(options && !options->is_empty()){
		size_t valid_options=0;
		for(Regex_option *o=regex_option; o->key; o++)
			if(
				options->pos(o->key)!=STRING_NOT_FOUND
				|| (o->keyAlt && options->pos(o->keyAlt)!=STRING_NOT_FOUND)
			){
				*o->result &= ~o->clear;
				*o->result |= o->set;
				valid_options++;
			}
		if(options->length()!=valid_options)
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}
}


void VRegex::set(Charset& acharset, const String* aregex, const String* aoptions){
	if(aregex->is_empty())
		throw Exception(PARSER_RUNTIME, 0, "regexp is empty");

	fcharset=&acharset;

	fpattern=aregex->untaint_cstr(String::L_REGEX);

	foptions_cstr=aoptions ? aoptions->cstr() : 0;

	regex_options(aoptions, foptions);
}


void VRegex::set(VRegex& avregex){
	fcharset=avregex.fcharset;

	fpattern=avregex.fpattern;

	foptions_cstr=avregex.foptions_cstr;

	regex_options(foptions_cstr ? new String(foptions_cstr) : 0, foptions);
}


void VRegex::compile(){
	const char* err_ptr;
	int err_offset;
	int options=foptions[0];

	// @todo (for UTF-8): check string & pattern and use PCRE_NO_UTF8_CHECK option 
	if(fcharset->isUTF8())
		options |= (PCRE_UTF8 | PCRE_UCP);

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
}


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
		prestart>0 ? PCRE_NO_UTF8_CHECK : 0, ovector, ovector_size);
			
	if(result<0 && result!=PCRE_ERROR_NOMATCH){
		throw Exception(PCRE_EXCEPTION_TYPE, 
			new String(fpattern, String::L_TAINTED),
			get_pcre_exec_error_text(result), result);
	}

	return result;
}


Value* VRegex::get_element(const String& aname) { 
	if(aname == REGEX_PATTERN_NAME)
		return new VString(*new String(fpattern, String::L_TAINTED));

	if(aname == REGEX_OPTIONS_NAME)
		return new VString(*new String(foptions_cstr, String::L_TAINTED));

	// method (if any)
	if(Value* result=VStateless_object::get_element(aname))
		return result;

	throw Exception(PARSER_RUNTIME,
		&aname,
		"reading of invalid field");
}
