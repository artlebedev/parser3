/** @file
	Parser: @b hashfile parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhashfile.h,v 1.1 2001/10/22 12:30:24 parser Exp $
*/

#ifndef PA_VHASHFILE_H
#define PA_VHASHFILE_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"

extern Methoded *hash_base_class;

/// value of type 'hashfile', implemented with GDBM library
class VHashfile : public VStateless_class {
public: // value

	const char *type() const { return "hashfile"; }

	/// VHashfile: true
	bool is_defined() const { return true; }

	/// VHashfile: convert to VHash
	Hash *get_hash();

	/// VHashfile: 0
	VStateless_class *get_class() { return 0; }

	/// VHashfile: (key)=value
	Value *get_element(const String& name) { 
		// $CLASS,$method
		if(Value *result=VStateless_class::get_element(name))
			return result;
		
		// $element
		return get_field(name);
	}
	
	/// VHashfile: (key)=value
	void put_element(const String& name, Value *value) {
		put_field(name, value);
	}


public: // usage

	VHashfile(Pool& apool) : VStateless_class(apool, hash_base_class),
		ffile_spec(0), ffile_spec_cstr(0) {
	}

	void set_file_spec(const String& afile_spec) { 
		ffile_spec=&afile_spec; 
		ffile_spec_cstr=afile_spec.cstr(String::UL_FILE_SPEC);
	}
	const String& get_file_spec(const String *source) const { 
		if(!ffile_spec)
			throw Exception(0, 0,
				source,
				"can not be applied to uninitialized instance");

		return *ffile_spec;
	}
	const char *get_file_spec_cstr(const String *source) {
		if(!ffile_spec_cstr)
			throw Exception(0, 0,
				source,
				"can not be applied to uninitialized instance");

		return ffile_spec_cstr;
	}

private:

	void check(const char *operation, const String *source, int error);

	Value *get_field(const String& name);
	
	void put_field(const String& name, Value *value);

private:

	const String *ffile_spec;
	const char *ffile_spec_cstr;

};

#endif
