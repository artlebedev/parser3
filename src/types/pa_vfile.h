/** @file
	Parser: file object decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vfile.h,v 1.14 2001/03/28 14:07:19 paf Exp $
*/

#ifndef PA_VFILE_H
#define PA_VFILE_H

#include "pa_common.h"
#include "pa_globals.h"
#include "pa_vstateless_object.h"
#include "_file.h"

/** holds received file.

	@see VForm
*/
class VFile : public VStateless_object {
public: // Value
	
	/// all: for error reporting after fail(), etc
	const char *type() const { return "file"; }

	/// VFile: this
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

	/// VFile: true
	bool as_bool() { return true; }

	/// VFile: CLASS,BASE,method,field
	Value *VFile::get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(aname))
			return result;

		// $name, $size, $text
		return static_cast<Value *>(fields.get(aname));
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool are_static_calls_disabled() { return true; }

public: // usage

	VFile::VFile(Pool& apool) : VStateless_object(apool, *file_class),
		fvalue_ptr(0),
		fvalue_size(0),
		fields(apool) {
	}

	void set(const void *avalue_ptr, size_t avalue_size, const char *afile_name,
		const String *amime_type=0);
	
	void save(const String& file_spec) {
		if(fvalue_ptr)
			file_write(pool(), file_spec, fvalue_ptr, fvalue_size, false);
		else
			THROW(0, 0,
				&file_spec,
				"saving unassigned file"); //never
	}

private:

	const void *fvalue_ptr;
	size_t fvalue_size;
	Hash fields;

};

#endif
