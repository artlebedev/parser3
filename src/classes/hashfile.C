/** @file
	Parser: @b hashfile parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hashfile.C,v 1.2 2001/10/22 16:44:42 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "classes.h"
#include "pa_request.h"
#include "pa_vhashfile.h"
#include "pa_vhash.h"

// defines

#define HASH_CLASS_NAME "hashfile"

// class

class MHashfile : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VHashfile(pool); }

public:
	MHashfile(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _assign(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VHashfile& self=*static_cast<VHashfile *>(r.self);
	
	const String &sfile_spec=params->as_string(0, "filename must be string");

	self.assign(r.absolute(sfile_spec), method_name);
}
//static_cast<VHashfile *>(r.self)	const String& file_spec=->file_spec();

// constructor

MHashfile::MHashfile(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), HASH_CLASS_NAME));

	// ^hashfile::assign[filename]
	add_native_method("assign", Method::CT_DYNAMIC, _assign, 1, 1);
/*
	// ^hashfile.delete[key]
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
	// ^hashfile.clear[]
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 0);
	// ^cache[key](seconds){code}
	add_native_method("cache", Method::CT_DYNAMIC, _cache, 3, 3);
	// ^cancel[]
	add_native_method("cancel", Method::CT_DYNAMIC, _cancel, 0, 0);
	// ^lock{code}
	add_native_method("lock", Method::CT_DYNAMIC, _lock, 1, 1);
	// ^pack[]
	add_native_method("pack", Method::CT_DYNAMIC, _pack, 0, 0);
	*/
}

// global variable

Methoded *hashfile_base_class;

// creator

Methoded *MHashfile_create(Pool& pool) {
	return hashfile_base_class=new(pool) MHashfile(pool);
}

#endif
