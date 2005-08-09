/** @file
	Parser: @b file parser type decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFILE_H
#define PA_VFILE_H

static const char * const IDENT_VFILE_H="$Date: 2005/08/09 08:14:54 $";

// include

#include "pa_common.h"
#include "pa_globals.h"
#include "pa_vstateless_object.h"
#include "pa_vbool.h"

// defines

#define NONAME_DAT "noname.dat"

// forwards

class Methoded;

/** holds received from user or read from disk file.

	@see VForm
*/
class VFile: public VStateless_object {

	const char* ffile_name_cstr;
	const char* fvalue_ptr;
	size_t fvalue_size;
	HashStringValue ffields;

public: // Value
	
	override const char* type() const { return "file"; }
	override VStateless_class *get_class();

	/// VFile: true
	override bool as_bool() const { return true; }

	/// VFile: true
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return *new VBool(as_bool()); }

	/// VFile: this
	override VFile* as_vfile(String::Language /*lang*/, 
		const Request_charsets* /*charsets*/) { return this; }

	/// VFile: method,field
	override Value* get_element(const String& aname, Value& aself, bool looking_up);

public: // usage

	VFile::VFile(): fvalue_ptr(0), fvalue_size(0) {}

	VFile::VFile(HashStringValue& afields): ffields(afields) {}

	/// WARNING: when setting text files be sure to append terminating zero to avalue_ptr
	void set(
		bool tainted, 
		const char* avalue_ptr, size_t avalue_size,
		const char* afile_name_cstr=0,
		Value* acontent_type=0);
	
	void save(const String& file_spec, bool is_text) {
		if(fvalue_ptr)
			file_write(file_spec, fvalue_ptr, fvalue_size, is_text);
		else
			throw Exception("parser.runtime",
				&file_spec,
				"saving stat-ed file");
	}

	const char* value_ptr() const { 
		if(!fvalue_ptr)
			throw Exception("parser.runtime",
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
