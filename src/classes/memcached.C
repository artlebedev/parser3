/** @file
	Parser: memcached class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors:
		Ivan Poluyanov <ivan-poluyanov@yandex.ru>
		Artem Stepanov <timofei1394@thesecure.in>
*/

#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vstring.h"
#include "pa_vtable.h"
#include "pa_vbool.h"
#include "pa_vmemcached.h"

volatile const char * IDENT_MEMCACHED_C="$Id: memcached.C,v 1.16 2020/12/15 17:10:29 moko Exp $";

class MMemcached: public Methoded {
public: // VStateless_class
	Value* create_new_value(Pool&) { return new VMemcached(); }
public:
	MMemcached();
};

DECLARE_CLASS_VAR(memcached, new MMemcached);

static void _open(Request& r, MethodParams& params) {
	VMemcached& self=GET_SELF(r, VMemcached);
	Value& param_value=params.as_no_junction(0, PARAM_MUST_NOT_BE_CODE);
	time_t ttl=params.count()>1 ? params.as_int(1, "default expiration must be int", r) : 0;

	if(HashStringValue* options=param_value.get_hash()){
		bool connect=true;
		String result;
		for(HashStringValue::Iterator i(*options); i; i.next()){
			if(i.key() == "skip-connect"){
				connect=!i.value()->as_bool();
			} else if(Value *b=i.value()->as("bool")){
				if(b->as_bool())
					result << (result.is_empty() ? "--" : " --") << i.key();
			} else {
				const String& value=i.value()->as_string();
				if(!value.is_empty())
					result << (result.is_empty() ? "--" : " --") << i.key() << "=" << value;
			}
		}
		self.open(result, ttl, connect);
	} else {
		const String& connect_string=params.as_string(0, "param must be connection string or options hash");
		self.open_parse(connect_string, ttl);
	}
}

static void _clear(Request& r, MethodParams& params) {
	VMemcached& self=GET_SELF(r, VMemcached);
	time_t ttl=(params.count()>0) ? params.as_int(0, "expiration must be int", r) : 0;

	self.flush(ttl);
}

static void _mget(Request& r, MethodParams& params) {
	VMemcached& self=GET_SELF(r, VMemcached);
	Value& param=params.as_no_junction(0, PARAM_MUST_NOT_BE_CODE);

	if(param.is_string()){

		ArrayString keys(params.count());
		
		for(size_t i=0; i<params.count(); i++) {
			keys+=&params.as_string(i, "key must be string");
		}

		r.write(self.mget(keys));
	} else {
		Table* table=param.get_table();
		if(table==0){
			throw Exception("memcached", 0, "key must be string or table");
		}
		
		ArrayString keys(table->count());
		
		for(size_t i=0; i<table->count(); i++) {
			keys+=table->get(i)->get(0);
		}

		r.write(self.mget(keys));
	}
}

static void _add(Request& r, MethodParams& params) {
	VMemcached& self=GET_SELF(r, VMemcached);
	const String& key=params.as_string(0, "key must be string");

	r.write(VBool::get(self.add(key, &params.as_no_junction(1, PARAM_MUST_NOT_BE_CODE))));
}

static void _delete(Request& r, MethodParams& params) {
	VMemcached& self=GET_SELF(r, VMemcached);
	const String& key=params.as_string(0, "key must be string");

	self.remove(key);
}

static void _release(Request& r, MethodParams&) {
	VMemcached& self=GET_SELF(r, VMemcached);

	self.quit();
}

MMemcached::MMemcached() : Methoded("memcached") {
	add_native_method("open", Method::CT_DYNAMIC, _open, 1, 2);
	add_native_method("clear", Method::CT_DYNAMIC, _clear, 0, 1);
	add_native_method("mget", Method::CT_DYNAMIC, _mget, 1, 1000);
	add_native_method("add", Method::CT_DYNAMIC, _add, 2, 2);
	add_native_method("delete", Method::CT_DYNAMIC, _delete, 1, 1);
	add_native_method("release", Method::CT_DYNAMIC, _release, 0, 0);
}
