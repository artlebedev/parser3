/** @file
	Parser: @b file parser type decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFILE_H
#define PA_VFILE_H

static const char* IDENT_VFILE_H="$Date: 2002/08/06 09:08:00 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_globals.h"
#include "pa_vstateless_object.h"

extern Methoded *file_class;

/** holds received from user or read from disk file.

	@see VForm
*/
class VFile : public VStateless_object {
public: // Value
	
	const char *type() const { return "file"; }
	VStateless_class *get_class() { return file_class; }

	/// VFile: true
	bool as_bool() const { return true; }

	/// VFile: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VFile: this
	VFile *as_vfile(String::Untaint_lang lang=String::UL_UNSPECIFIED,
		bool origins_mode=false) { 
		return this; 
	}

	/// VFile: CLASS,method,field
	Value *VFile::get_element(const String& aname) {
		// $CLASS,$method
		if(Value *result=VStateless_object::get_element(aname))
			return result;

		// $name, $size, $text
		return static_cast<Value *>(ffields.get(aname));
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VFile::VFile(Pool& apool) : VStateless_object(apool),
		fvalue_ptr(0),
		fvalue_size(0),
		ffields(apool) {
	}

	void set(bool tainted, 
		const void *avalue_ptr, size_t avalue_size, const char *afile_name=0,
		Value *acontent_type=0);
	
	void save(const String& file_spec, bool is_text) {
		if(fvalue_ptr)
			file_write(file_spec, fvalue_ptr, fvalue_size, is_text);
		else
			throw Exception("parser.runtime",
				&file_spec,
				"saving stat-ed file");
	}

	const void *value_ptr() const { 
		if(!fvalue_ptr)
			throw Exception("parser.runtime",
				0,
				"getting value of stat-ed file");

		return fvalue_ptr; 
	}
	size_t value_size() const { return fvalue_size; }
	Hash& fields() { return ffields; }
	const Hash& fields() const { return ffields; }

private:

	Value *fields_element();

private:

	const void *fvalue_ptr;
	size_t fvalue_size;
	Hash ffields;

};

#endif
