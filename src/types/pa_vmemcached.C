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
#include "pa_vhash.h"
#include "pa_vvoid.h"

volatile const char * IDENT_PA_VMEMCACHED_C="$Id: pa_vmemcached.C,v 1.6 2012/04/13 22:59:36 moko Exp $" IDENT_PA_VMEMCACHED_H;

#ifdef WIN32
const char *memcached_library="libmemcached.dll";
#else
const char *memcached_library="libmemcached.so";
#endif

void error(const char *step, memcached_st* m, memcached_return rc) {
	const char* str=f_memcached_strerror(m, rc);
	throw Exception("memcached", 0, "%s error: %s (%d)", step, str ? str : "<unknown>", rc);
}

inline void check(const char *action, memcached_st* m, memcached_return rc) {
	if(rc==MEMCACHED_SUCCESS)
		return;
	error(action, m, rc);
}

inline void check_key(const String& akey) {
	if(akey.is_empty())
		throw Exception("memcached", 0, "key must not be empty");
	if(akey.length() > MEMCACHED_MAX_KEY)
		throw Exception("memcached", &akey, "key length %d exceeds limit (%d bytes)", akey.length(), MEMCACHED_MAX_KEY);
}

void VMemcached::open(const String& connect_string, time_t attl){
	const char *library = memcached_library;
	const char *memcached_status = memcached_load(library);

	if(memcached_status)
		throw Exception("memcached", 0, "failed to load memcached library %s: %s", library, memcached_status);

	if(connect_string.is_empty())
		throw Exception("memcached", 0, "server name must not be empty");

	fttl=attl;

	fm=f_memcached_create(NULL);

	memcached_server_st* fservers = f_memcached_servers_parse(connect_string.cstr());
	check("server_push", fm, f_memcached_server_push(fm, fservers));
}

void VMemcached::flush(time_t attl) {
	check("flush", fm, f_memcached_flush(fm, attl));
}

void VMemcached::remove(const String& aname) {
	check_key(aname);

	memcached_return rc=f_memcached_delete(fm, aname.cstr(), aname.length(), (time_t)0);

	if(rc != MEMCACHED_SUCCESS && rc != MEMCACHED_NOTFOUND)
		error("delete", fm, rc);
}

Value* VMemcached::get_element(const String& aname) {
	if(Value *result=VStateless_object::get_element(aname))
		return result;

	check_key(aname);

	memcached_return rc;
	Serialization_data data;
	data.ptr=f_memcached_get(fm, aname.cstr(), aname.length(), &data.length, &data.flags, &rc);

	if(rc==MEMCACHED_SUCCESS){
		data.ptr=pa_strdup(data.ptr, data.length);
		return &memcached_deserialize(data);
	}
	
	if(rc==MEMCACHED_NOTFOUND)
		return new VVoid();

	error("get", fm, rc);
	return 0; // calm down compiler
}

Value &VMemcached::mget(ArrayString& akeys) {
	VHash &hresult = *new VHash();
	
	size_t kl = akeys.count();
	
	if(kl==0)
		return hresult;
	
	const char **keys = new const char *[kl];
	size_t *key_lengths = new size_t[kl];
	
	for(size_t i=0; i<kl; i++){
		const String &skey = *(akeys[i]);
		check_key(skey);
		keys[i] = skey.cstr();
		key_lengths[i] = skey.length();
	}
	
	check("mget", fm, f_memcached_mget(fm, keys, key_lengths, kl));
	
	memcached_result_st *results=f_memcached_result_create(fm, 0);
	
	memcached_return rc;
	
	while(f_memcached_fetch_result(fm, results, &rc) && (rc == MEMCACHED_SUCCESS)){
		const char *hkey = pa_strdup(f_memcached_result_key_value(results), f_memcached_result_key_length(results));
		
		Serialization_data value(f_memcached_result_flags(results));
		value.length = f_memcached_result_length(results);
		value.ptr = pa_strdup(f_memcached_result_value(results), value.length);

		hresult.hash().put(hkey, &memcached_deserialize(value));
	}

	if (rc != MEMCACHED_END && rc != MEMCACHED_NOTFOUND)
		error("mget", fm, rc);
	
	delete keys;
	delete key_lengths;
	
//	f_memcached_result_free(results);

	return hresult;
}

const VJunction* VMemcached::put_element(const String& aname, Value* avalue, bool /*replace*/){
	check_key(aname);

	time_t ttl=fttl;
	Value* lvalue;

	if(HashStringValue* hash=avalue->get_hash()) {
		if(Value* ttl_value=hash->get(expires_name))
			ttl=ttl_value->as_int();
		if(lvalue=hash->get(value_name)){
			if(lvalue->get_junction())
				throw Exception("memcached", 0, VALUE_NAME " must not be code");
		} else
			throw Exception("memcached", &aname, "value hash must contain ." VALUE_NAME);
	} else {
		lvalue=avalue;
	}

	Serialization_data value;
	avalue->serialize(value);

	check("set", fm, f_memcached_set(
			fm,
			aname.cstr(),
			aname.length(),
			value.ptr,
			value.length,
			ttl,
			value.flags));

	return PUT_ELEMENT_REPLACED_ELEMENT;
}

