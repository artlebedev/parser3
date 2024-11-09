/** @file
	Parser: @b regex class decls.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VREGEX_H
#define PA_VREGEX_H

#define IDENT_PA_VREGEX_H "$Id: pa_vregex.h,v 1.18 2024/11/09 17:14:40 moko Exp $"

// include

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_charset.h"

// defines

#define VREGEX_TYPE "regex"

enum Match_feature {
	MF_GLOBAL_SEARCH = 0x01,
	MF_NEED_PRE_POST_MATCH = 0x02,
	MF_JUST_COUNT_MATCHES = 0x04
};

extern Methoded* regex_class;

// VRegex
class VRegex: public VStateless_object {

public: // Value

	override const char* type() const { return VREGEX_TYPE; }
	override VStateless_class *get_class() { return regex_class; }

	/// VRegex: PCRE_INFO_SIZE
	override int as_int() { return (int)get_info_size(); }

	/// VRegex: PCRE_INFO_SIZE
	override double as_double() { return (double)get_info_size(); }

	/// VRegex: scalar
	override Value& as_expr_result();

	/// VRegex: true
	virtual bool is_defined() const { return true; }

	/// VRegex: true
	override bool as_bool() const { return true; }

	override Value* get_element(const String& aname);

public: // usage

#ifdef HAVE_PCRE2
	static pcre2_general_context* fgen_ctxt;
#endif

	VRegex():
		fcharset(0),
		fpattern(0),
		foptions_cstr(0),
		fcode(0),
#ifdef HAVE_PCRE2
		fcmp_ctxt(0),
		fmatch_ctxt(0),
		fmatch_data(0)
#else
		fextra(0),
		fstudied(false)
#endif
	{
		foptions[0]=0;
		foptions[1]=0;
	}

	VRegex(Charset& acharset, const String* aregex, const String* aoptions):
#ifdef HAVE_PCRE2
		fcmp_ctxt(0),
		fmatch_ctxt(0),
		fmatch_data(0)
#else
		fextra(0),
		fstudied(false)
#endif
	{
		set(acharset, aregex, aoptions);
		compile();
	}

	~VRegex(){
#ifdef HAVE_PCRE2
		pcre2_match_data_free(fmatch_data);
		pcre2_match_context_free(fmatch_ctxt);
		pcre2_compile_context_free(fcmp_ctxt);
		pcre2_code_free(fcode);
#else
		if(fextra)
			pcre_free(fextra);
		if(fcode)
			pcre_free(fcode);
#endif
	}

	void set(Charset& acharset, const String* aregex, const String* aoptions);
	void set(VRegex& avregex);

	void compile();

	void study();

	int exec(const char* string, size_t string_len, int* ovector, int ovector_size, int prestart=0);

	// size_t info();

	size_t full_info(int type);

	size_t get_info_size();

	size_t get_study_size();

	size_t get_options();

	bool is_pre_post_match_needed(){
		return (foptions[1] & MF_NEED_PRE_POST_MATCH)!=0;
	}

	bool is_just_count(){
		return (foptions[1] & MF_JUST_COUNT_MATCHES)!=0;
	}

	bool is_global_search(){
		return (foptions[1] & MF_GLOBAL_SEARCH)!=0;
	}

private:
	static void regex_options(const String* options, int* result);

private:
	Charset* fcharset;
	const char* fpattern;
	const char* foptions_cstr;
	int foptions[2];

#ifdef HAVE_PCRE2
	pcre2_code* fcode;
	pcre2_compile_context* fcmp_ctxt;
	pcre2_match_context* fmatch_ctxt;
	pcre2_match_data* fmatch_data;
#else
	pcre* fcode;
	pcre_extra* fextra;
	bool fstudied;
#endif
};


class VRegexCleaner {
public:
	VRegex *vregex;

	VRegexCleaner(): vregex(0) { }

	~VRegexCleaner(){
		if(vregex) delete vregex;
	}

};


#endif
