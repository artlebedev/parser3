/** @file
	Parser: @b regex class decls.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VREGEX_H
#define PA_VREGEX_H

static const char * const IDENT_VREGEX_H="$Date: 2009/05/13 07:35:27 $";

// include

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_charset.h"
#include "pcre.h"

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

	/// VRegex: count
	override int as_int() { return get_info_size(); }

	/// VRegex: count
	override double as_double() { return as_int(); }

	/// VRegex: true
	override bool is_evaluated_expr() const { return true; }

	/// VRegex: scalar
	override Value& as_expr_result(bool/*return_string_as_is=false*/);

	/// VRegex: true
	virtual bool is_defined() const { return as_bool(); }

	/// VRegex: true
	override bool as_bool() const { return true; }

public: // usage

	VRegex():
		fcharset(0),
		fpattern(0),
		fcode(0),
		fextra(0),
		fstudied(false)
	{
		foptions[0]=0;
		foptions[1]=0;
	}

	VRegex(Charset& acharset, const String* aregex, const String* aoptions):
		fextra(0),
		fstudied(false)
	{
		set(acharset, aregex, aoptions);
		compile();
	}

	~VRegex(){
		if(fextra)
			pcre_free(fextra);
		if(fcode)
			pcre_free(fcode);
	}

	void set(Charset& acharset, const String* aregex, const String* aoptions);

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
	int foptions[2];

	pcre* fcode;
	pcre_extra* fextra;
	bool fstudied;
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
