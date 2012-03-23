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
#include "pa_vtable.h"
#include "pa_vmemcached.h"

volatile const char * IDENT_MEMCACHED_C="$Id: memcached.C,v 1.2 2012/03/23 22:33:23 moko Exp $";

class MMemcached: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VMemcached(); }
public:
	MMemcached();
};

DECLARE_CLASS_VAR(memcached, new MMemcached, 0);

static void _open(Request& r, MethodParams& params) {
	const String& connect_string=params.as_string(0, "connection string must be string");
	int ttl=0;
	
	if(params.count()>1)
		ttl=params.as_int(1, "default expiration must be int", r);

	VMemcached& self=GET_SELF(r, VMemcached);
	self.open(connect_string, ttl);
}

static void _flush(Request& r, MethodParams& params) {
	int ttl=0;

	if(params.count()>0)
		params.as_int(0, "expiration must be int", r);

	VMemcached& self=GET_SELF(r, VMemcached);
	self.flush(ttl);
}

static void _mget(Request& r, MethodParams& params) {
	Value& param=params.as_no_junction(0, PARAM_MUST_NOT_BE_CODE);

	VMemcached& self=GET_SELF(r, VMemcached);

	if(param.is_string()){

		ArrayString keys(params.count());
		
		for(size_t i=0; i<params.count(); i++) {
			keys+=&params.as_string(i, "key must be string");
		}

		r.write_no_lang(self.mget(keys));
	} else {
		Table* table=param.get_table();
		if(table==0){
			throw Exception("memcached", 0, "key must be string or table");
		}
		
		ArrayString keys(table->count());
		
		for(size_t i=0; i<table->count(); i++) {
			keys+=table->get(i)->get(0);
		}

		r.write_no_lang(self.mget(keys));
	}
}

static void _delete(Request& r, MethodParams& params) {
	const String& key=params.as_string(0, "key must be string");

	VMemcached& self=GET_SELF(r, VMemcached);
	self.remove(key);
}

MMemcached::MMemcached() : Methoded("memcached") {
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 2);
	add_native_method("flush", Method::CT_DYNAMIC, _flush, 1, 1);
	add_native_method("mget", Method::CT_DYNAMIC, _mget, 1, 1000);
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
}
