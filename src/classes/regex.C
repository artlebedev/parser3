/** @file
	Parser: @b int parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_REGEX_C="$Date: 2009/08/08 13:30:20 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vint.h"
#include "pa_vregex.h"

// class

class MRegex: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VRegex(); }
public:
	MRegex();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(regex, new MRegex, 0);

// methods

static void _create(Request& r, MethodParams& params) {
	const String& pattern=params.as_string(0, "regexp must not be code");

	VRegex& vregex=GET_SELF(r, VRegex);

	vregex.set(r.charsets.source(),
		&pattern,
		params.count()>1?&params.as_string(1, OPTIONS_MUST_NOT_BE_CODE):0);

	vregex.compile();

	vregex.study();
}


static void _size(Request& r, MethodParams&) {
	VRegex& vregex=GET_SELF(r, VRegex);
	r.write_no_lang(*new VInt(vregex.get_info_size()));
}

static void _study_size(Request& r, MethodParams&) {
	VRegex& vregex=GET_SELF(r, VRegex);
	r.write_no_lang(*new VInt(vregex.get_study_size()));
}

// constructor

MRegex::MRegex(): Methoded("regex") {
	// ^regex::create[string[;options]]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 2);

	// ^regex.info_size[]
	add_native_method("size", Method::CT_DYNAMIC, _size, 0, 0);

	// ^regex.study_size[]
	add_native_method("study_size", Method::CT_DYNAMIC, _study_size, 0, 0);

}

