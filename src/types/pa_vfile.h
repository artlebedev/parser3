/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vfile.h,v 1.1 2001/03/19 20:08:13 paf Exp $
*/

#ifndef PA_VFILE_H
#define PA_VFILE_H

#include "pa_common.h"
#include "pa_globals.h"
#include "pa_vstateless_class.h"
#include "_file.h"
#include "pa_vint.h"

class Request;

class VFile : public VStateless_class {
public: // Value
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "file"; }

	// file: CLASS,BASE,method,field
	Value *VFile::get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(aname))
			return result;

		// $element
		return static_cast<Value *>(fields.get(aname));
	}

public: // usage

	VFile(Pool& apool, 
		const char *avalue_ptr, size_t avalue_size,
		const char *afile_name) : 
		VStateless_class(apool, file_base_class),

		fvalue_ptr(avalue_ptr),
		fvalue_size(avalue_size),
		fields(apool) {

		// $name
		fields.put(*name_name, NEW VString(*NEW String(pool(), afile_name, true)));
		// $size
		fields.put(*size_name, NEW VInt(pool(), fvalue_size));
		// $text
		String& text=*NEW String(pool());
		text.APPEND(fvalue_ptr, fvalue_size, "<input type=file>", 0);
		fields.put(*text_name, NEW VString(text));

	}

	void save(const char *file_spec) {
		file_write(pool(), file_spec, fvalue_ptr, fvalue_size);
	}

private:

	const char *fvalue_ptr;
	size_t fvalue_size;
	Hash fields;

};

#endif
