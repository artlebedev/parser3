/** @file
	Parser: @b regex class.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vregex.h"
#include "pa_vint.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_VREGEX_C="$Id: pa_vregex.C,v 1.26 2025/05/26 01:56:54 moko Exp $" IDENT_PA_VREGEX_H;

// defines

#define REGEX_PATTERN_NAME "pattern"
#define REGEX_OPTIONS_NAME "options"

const char* get_pcre_exec_error_text(int exec_result){
	if(exec_result == PCRE_ERROR_BADUTF8_OFFSET ||
#ifdef HAVE_PCRE2
		exec_result <= PCRE2_ERROR_UTF8_ERR1 /* -3 */ && exec_result >= PCRE2_ERROR_UTF8_ERR21 /* -21 */
#else
		exec_result == PCRE_ERROR_BADUTF8
#endif
	)
		return "UTF-8 validation failed during pcre_exec (%d).";
	return "execution error (%d)";
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
	int options=foptions[0];

	// @todo (for UTF-8): check string & pattern and use PCRE_NO_UTF8_CHECK option 
	if(fcharset->isUTF8())
		options |= (PCRE_UTF8 | PCRE_UCP);

#ifdef HAVE_PCRE2
	int err;
	size_t err_offset;
	PCRE2_UCHAR buffer[120];

	if(!fcmp_ctxt)
		fcmp_ctxt=pcre2_compile_context_create(fgen_ctxt);

	pcre2_set_character_tables(fcmp_ctxt, fcharset->pcre_tables);

	fcode=pcre2_compile((PCRE2_SPTR)fpattern, PCRE2_ZERO_TERMINATED, options,
		&err, &err_offset,
		fcmp_ctxt);

	if(!fcode){
		pcre2_get_error_message(err, buffer, sizeof(buffer));
		err_ptr=(const char*)buffer;
	}
#else
	int err_offset;
	fcode=pcre_compile(fpattern, options,
		&err_ptr, &err_offset,
		fcharset->pcre_tables);
#endif
	if(!fcode){
		throw Exception(PCRE_EXCEPTION_TYPE,
			new String(fpattern + (fpattern[err_offset] ? err_offset : 0), String::L_TAINTED),
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
#ifdef HAVE_PCRE2
	return 0;
#else
	return full_info(PCRE_INFO_STUDYSIZE);
#endif
}


void VRegex::study(){
#ifndef HAVE_PCRE2
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
#endif
}


int VRegex::exec(const char* string, size_t string_len, int* ovector, int ovector_size, int prestart){
#ifdef HAVE_PCRE2
	if(!fmatch_ctxt)
		fmatch_ctxt=pcre2_match_context_create(fgen_ctxt);

	if(!fmatch_data)
		fmatch_data=pcre2_match_data_create_from_pattern(fcode, fgen_ctxt);

	int result=pcre2_match(fcode,
		(PCRE2_SPTR)string, string_len, prestart,
		prestart>0 ? PCRE2_NO_UTF_CHECK : 0, fmatch_data, fmatch_ctxt);
#else
	int result=pcre_exec(fcode, fextra, 
		string, string_len, prestart,
		prestart>0 ? PCRE_NO_UTF8_CHECK : 0, ovector, ovector_size);
#endif

	if(result<0 && result!=PCRE_ERROR_NOMATCH){
		throw Exception(PCRE_EXCEPTION_TYPE, 
			new String(fpattern, String::L_TAINTED),
			get_pcre_exec_error_text(result), result);
	}

#ifdef HAVE_PCRE2
	if(result>0){
		result=min(result, ovector_size/3);
		size_t* groups=pcre2_get_ovector_pointer(fmatch_data);
		for(int i=0; i<result*2; i++){
			ovector[i]=groups[i];
		}
	}
#endif
	return result;
}


Value* VRegex::get_element(const String& aname) { 
	if(aname == REGEX_PATTERN_NAME)
		return new VString(fpattern);

	if(aname == REGEX_OPTIONS_NAME)
		return new VString(foptions_cstr);

	// method (if any)
	if(Value* result=VStateless_object::get_element(aname))
		return result;

	return bark("%s field not found", &aname);
}
