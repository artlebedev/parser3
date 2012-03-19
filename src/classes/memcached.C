/** @file
	Parser: memcached class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors:
		Ivan Poluyanov <ivan-poluyanov@yandex.ru>
		Artem Stepanov <timofei1394@thesecure.in>
*/

#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vmemcached.h"

volatile const char * IDENT_MEMCACHED_C="$Id: memcached.C,v 1.1 2012/03/19 22:19:27 moko Exp $";

class MMemcached: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VMemcached(); }
public:
	MMemcached();
};

DECLARE_CLASS_VAR(memcached, new MMemcached, 0);

static void _open(Request& r, MethodParams& params) {
	const String& connect_string=params.as_string(0, "connection string must be string");
	VMemcached& self=GET_SELF(r, VMemcached);
	self.open(connect_string);
}

static void _delete(Request& r, MethodParams& params) {
	const String& key=params.as_string(0, "key must be string");

	VMemcached& self=GET_SELF(r, VMemcached);
	self.remove(key);
}

MMemcached::MMemcached() : Methoded("memcached") {
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 1);
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
}
