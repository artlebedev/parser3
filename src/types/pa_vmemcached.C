/** @file
	Parser: memcached class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors:
		Ivan Poluyanov <ivan-poluyanov@yandex.ru>
		Artem Stepanov <timofei1394@thesecure.in>
*/

#include "pa_vmemcached.h"
#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vvoid.h"

volatile const char * IDENT_PA_VMEMCACHED_C="$Id: pa_vmemcached.C,v 1.1 2012/03/19 22:22:59 moko Exp $" IDENT_PA_VMEMCACHED_H;

#ifdef WIN32
const char *memcached_library="libmemcached.dll";
#else
const char *memcached_library="libmemcached.so";
#endif

void error(const char *step, memcached_st* m, memcached_return rc) {
	const char* str=f_memcached_strerror(m, rc);
	throw Exception("memcached", 0, "%s error: %s (%d)", step, str ? str : "<unknown>", rc);
}

inline void check(const char *step, memcached_st* m, memcached_return rc) {
	if(rc==MEMCACHED_SUCCESS)
		return;
	error(step, m, rc);
}

void VMemcached::open(const String& connect_string, size_t aflags){
	const char *library = memcached_library;
	const char *memcached_status = memcached_load(library);

	if(memcached_status)
		throw Exception("memcached", 0, "failed to load memcached library %s: %s", library, memcached_status);

	if(connect_string.is_empty())
		throw Exception("memcached", 0, "server name must not be empty");

	fflags=aflags;

	fm=f_memcached_create(NULL);

	memcached_server_st* fservers = f_memcached_servers_parse(connect_string.cstr());
	check("server_push", fm, f_memcached_server_push(fm, fservers));
}

void VMemcached::remove(const String& aname){
	if(aname.is_empty())
		throw Exception("memcached", 0, "key must not be empty");
	const char* key_cstr=aname.cstr();

	memcached_return rc=f_memcached_delete(fm, key_cstr, strlen(key_cstr), (time_t)0);
	if(rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOTFOUND)
		error("delete", fm, rc);
}

Value* VMemcached::get_element(const String& aname) {
	if(Value *result=VStateless_object::get_element(aname))
		return result;

	const char* key=aname.cstr();

	size_t length;
	memcached_return rc;
	const char* val=f_memcached_get(fm, key, strlen(key), &length, &fflags, &rc);

	if(rc==MEMCACHED_SUCCESS)
		return new VString(*new String(val, String::L_TAINTED, length));
	else if(rc==MEMCACHED_NOTFOUND)
		return new VVoid();

	error("get", fm, rc);
	return 0; // calm down compiler
}


const VJunction* VMemcached::put_element(const String& aname, Value* avalue, bool /*replace*/){
	if(aname.is_empty())
		throw Exception("memcached", 0, "key must not be empty");

	Value* lvalue=0;
	time_t ttl=0;

	if(HashStringValue* hash=avalue->get_hash()) {
		if(Value* ttl_value=hash->get(expires_name))
			ttl=ttl_value->as_int();
		if(lvalue=hash->get(value_name)){
			if(lvalue->get_junction())
				throw Exception("memcached", 0, VALUE_NAME " must not be code");
		} else
			throw Exception("memcached", &aname, "put hash value must contain ." VALUE_NAME);
	} else {
		lvalue=avalue;
	}

	const char* key=aname.cstr();
	size_t key_size=strlen(key);

	if(key_size >= MEMCACHED_MAX_KEY)
		throw Exception("memcached", 0, "key length exceeds limit (%d bytes)", MEMCACHED_MAX_KEY-1);

	const char* value = avalue->as_string().cstr();
	size_t value_size=strlen(value);

	check("set", fm, f_memcached_set(
			fm,
			key,
			key_size,
			value,
			value_size,
			ttl,
			fflags));

	return PUT_ELEMENT_REPLACED_ELEMENT;
}

