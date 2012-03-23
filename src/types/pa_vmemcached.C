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

volatile const char * IDENT_PA_VMEMCACHED_C="$Id: pa_vmemcached.C,v 1.2 2012/03/23 22:33:23 moko Exp $" IDENT_PA_VMEMCACHED_H;

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

void VMemcached::flush(time_t attl){
	check("flush", fm, f_memcached_flush(fm, attl));
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

	if(aname.is_empty())
		throw Exception("memcached", 0, "key must not be empty");

	size_t length;
	uint32_t flags;
	memcached_return rc;
	const char* val=f_memcached_get(fm, aname.cstr(), aname.length(), &length, &flags, &rc);

	if(rc==MEMCACHED_SUCCESS)
		// we can't use length from memcached as there can be '\0' inside
		return new VString(*new String(val, String::L_TAINTED));
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
	size_t *key_length = new size_t[kl];
	
	for(int i=0; i<kl; i++){
		const String *skey = akeys[i];
		if(skey->is_empty())
			throw Exception("memcached", 0, "key must not be empty");
		keys[i] = skey->cstr();
		key_length[i] = skey->length();
	}
	
	check("mget", fm, f_memcached_mget(fm, keys, key_length, kl));
	
	memcached_result_st *results=f_memcached_result_create(fm, 0);
	
	for(;;){
		memcached_return rc;
		memcached_result_st *res = f_memcached_fetch_result(fm, results, &rc);
		if (rc == MEMCACHED_SUCCESS){
			// we can't use length from memcached as there can be '\0' inside
			String::Body hkey(f_memcached_result_key_value(res), String::L_TAINTED);
			String &hvalue = *new String(f_memcached_result_value(res), String::L_TAINTED);
			hresult.hash().put(hkey, new VString(hvalue));
		} else {
			if (rc != MEMCACHED_END)
				error("mget", fm, rc);
			break;
		}
	}
	
//	f_memcached_result_free(results);

	return hresult;
}

const VJunction* VMemcached::put_element(const String& aname, Value* avalue, bool /*replace*/){
	if(aname.is_empty())
		throw Exception("memcached", 0, "key must not be empty");

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

	const char* key=aname.cstr();
	size_t key_length=aname.length();

	if(key_length > MEMCACHED_MAX_KEY)
		throw Exception("memcached", &aname, "key length %d exceeds limit (%d bytes)", key_length, MEMCACHED_MAX_KEY);

	const String &value=avalue->as_string();

	check("set", fm, f_memcached_set(
			fm,
			key,
			key_length,
			value.cstr(),
			value.length(),
			ttl,
			0));

	return PUT_ELEMENT_REPLACED_ELEMENT;
}

