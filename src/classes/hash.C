/** @file
	Parser: @b hash parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: hash.C,v 1.1 2001/05/10 14:00:18 paf Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vhash.h"
#include "pa_vunknown.h"

// class

class MHash : public Methoded {
public:
	MHash(Pool& pool);
public: // Methoded
	bool used_directly() { return false; }
};

// methods

static void _default(Request& r, const String&, MethodParams *params) { 
	Pool& pool=r.pool();

	VHash& vhash=*static_cast<VHash *>(r.self);
	if(params->size())
		vhash.set_default(params->get(0)); // info: may be code..
	else {
		Value *default_value=vhash.get_default();
		r.write_assign_lang(default_value?*default_value:*new(pool) VUnknown(pool));
	}
}

// constructor

MHash::MHash(Pool& apool) : Methoded(apool) {
	// ^hash.default[]
	// ^hash.default[hash]
	add_native_method("default", Method::CT_DYNAMIC, _default, 0, 1);
}

// global variable

Methoded *hash_base_class;

// creator

Methoded *MHash_create(Pool& pool) {
	return hash_base_class=new(pool) MHash(pool);
}
