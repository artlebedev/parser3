/** @file
	Parser: @b file parser type decl.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFILE_H
#define PA_VFILE_H

static const char * const IDENT_VFILE_H="$Date: 2009/06/24 09:02:44 $";

// include

#include "pa_common.h"
#include "pa_globals.h"
#include "pa_vstateless_object.h"
#include "pa_vbool.h"

// defines

#define NONAME_DAT "noname.dat"
#define VFILE_TYPE "file"

#define TEXT_MODE_NAME "text"
#define BINARY_MODE_NAME "binary"

static const String text_mode_name(TEXT_MODE_NAME);
static const String binary_mode_name(BINARY_MODE_NAME);

// forwards

class Methoded;

/** holds received from user or read from disk file.

	@see VForm
*/
class VFile: public VStateless_object {

	const char* ffile_name_cstr;
	const char* fvalue_ptr;
	size_t fvalue_size;
	bool ftext_tainted;
	HashStringValue ffields;

public: // Value
	
	override const char* type() const { return VFILE_TYPE; }
	override VStateless_class *get_class();

	/// VFile: true
	override bool as_bool() const { return true; }

	/// VFile: true
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return VBool::get(as_bool()); }

	/// VFile: this
	override VFile* as_vfile(String::Language /*lang*/, 
		const Request_charsets* /*charsets*/) { return this; }

	/// VFile: method,field
	override Value* get_element(const String& aname, Value& aself, bool looking_up);

public: // usage

	VFile(): fvalue_ptr(0), fvalue_size(0) {}

	VFile(HashStringValue& afields): ffields(afields) {}

	/// WARNING: when setting text files be sure to append terminating zero to avalue_ptr
	void set(
		bool atainted, 
		const char* avalue_ptr, size_t avalue_size,
		const char* afile_name_cstr=0,
		Value* acontent_type=0);

	void set_mode(bool as_text);
	
	void save(const String& file_spec, bool is_text) {
		if(fvalue_ptr)
			file_write(file_spec, fvalue_ptr, fvalue_size, is_text);
		else
			throw Exception(PARSER_RUNTIME,
				&file_spec,
				"saving stat-ed file");
	}

	const char* value_ptr() const { 
		if(!fvalue_ptr)
			throw Exception(PARSER_RUNTIME,
				0,
				"getting value of stat-ed file");

		return fvalue_ptr; 
	}
	size_t value_size() const { return fvalue_size; }
	HashStringValue& fields() { return ffields; }

private:

	Value* fields_element();

};

#endif
