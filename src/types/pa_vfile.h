/** @file
	Parser: file object decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vfile.h,v 1.9 2001/03/27 15:37:54 paf Exp $
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
	Value *get_expr_result(bool return_string_as_is=false) { return this; }
	/// VFile: true
	bool get_bool() { return true; }

	/// VFile: CLASS,BASE,method,field
	Value *VFile::get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_object::get_element(aname))
			return result;

		// $element
		return static_cast<Value *>(fields.get(aname));
	}

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_class_alias
	VStateless_class *get_class_alias() { return 0; }

public: // usage

	VFile(Pool& apool, 
		const char *avalue_ptr, size_t avalue_size,
		const char *afile_name);

	void save(const String& file_spec) {
		file_write(pool(), file_spec, fvalue_ptr, fvalue_size, false);
	}

private:

	const char *fvalue_ptr;
	size_t fvalue_size;
	Hash fields;

};

#endif
