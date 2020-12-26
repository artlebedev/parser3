/** @file
	Parser: memcached class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors:
		Ivan Poluyanov <ivan-poluyanov@yandex.ru>
		Artem Stepanov <timofei1394@thesecure.in>
*/

#include "pa_vmemcached.h"
#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_vvoid.h"

volatile const char * IDENT_PA_VMEMCACHED_C="$Id: pa_vmemcached.C,v 1.23 2020/12/26 23:09:08 moko Exp $" IDENT_PA_VMEMCACHED_H;

const char *memcached_library="libmemcached" LT_MODULE_EXT;

// support functions

static void error(const char *step, memcached_st* m, memcached_return rc) {
	const char* str=f_memcached_strerror(m, rc);
	throw Exception("memcached", 0, "%s error: %s (%d)", step, str ? str : "<unknown>", rc);
}

inline void check(const char *action, memcached_st* m, memcached_return rc) {
	if(rc==MEMCACHED_SUCCESS)
		return;
	error(action, m, rc);
}

inline void check(const char *action, memcached_st* m, memcached_return rc, memcached_return ok) {
	if(rc==MEMCACHED_SUCCESS || rc==ok)
		return;
	error(action, m, rc);
}

inline void check_key(const String& akey) {
	if(akey.is_empty())
		throw Exception("memcached", 0, "key must not be empty");
	if(akey.length() > MEMCACHED_MAX_KEY)
		throw Exception("memcached", &akey, "key length %d exceeds limit (%d bytes)", akey.length(), MEMCACHED_MAX_KEY);
}

// serialization helpers

#define SERIALIZED_STRING 256

struct Serialization_data{
    unsigned int flags;
    const char *ptr;
    size_t length;

    Serialization_data() : flags(0), ptr(0), length(0){}
    Serialization_data(unsigned int aflags) : flags(aflags), ptr(0), length(0){}
    Serialization_data(unsigned int aflags, const char *aptr, size_t alength) : flags(aflags), ptr(aptr), length(alength){}
};

static void serialize_string(const String &str, Serialization_data &data){
	if(str.is_empty()){
		data = Serialization_data(SERIALIZED_STRING);
		return;
	}
	
	if (str.is_not_just_lang()){
		String::Cm cm = str.serialize(0);
		data = Serialization_data(SERIALIZED_STRING, cm.str, cm.length);
	} else {
		data = Serialization_data(SERIALIZED_STRING + (unsigned int)str.just_lang(), str.cstr(), str.length());
	}
}

static VString *deserialize_string(Serialization_data &data){
	String *result;

	if(data.flags==SERIALIZED_STRING){
		result = new String();
		if (data.length>0 && !result->deserialize(0, (void *)data.ptr, data.length))
			return NULL;
	} else {
		// we can't use length from memcached as there can be '\0' inside
		String::Language lang=(String::Language)(data.flags-SERIALIZED_STRING);
		result = new String(data.ptr, lang);
	}

	return new VString(*result);
}

static Value &deserialize(Serialization_data &data){
	Value *result=NULL;
	
	if(data.flags>=SERIALIZED_STRING && data.flags<(SERIALIZED_STRING+256)){
		// String->deserialize uses passed string
		if(data.length>0)
			data.ptr=pa_strdup(data.ptr, data.length);
		result=deserialize_string(data);
	}

	if (!result)
		throw Exception(PARSER_RUNTIME, 0, "unable to deserialize data id %d, size %d", data.flags, data.length);

	return *result;
}

// VMemcached

static void load_memcached(const char *library){
	const char *memcached_status = memcached_load(library);

	if(memcached_status)
		throw Exception("memcached", 0, "failed to load memcached library %s: %s", library, memcached_status);
}

void VMemcached::open(const String& options_string, time_t attl, bool connect){
	load_memcached(memcached_library);
	
	if(f_memcached==NULL)
		throw Exception("memcached", 0, "options hash requires libmemcached version 0.49 or later");
	
	if(options_string.is_empty())
		throw Exception("memcached", 0, "options hash must not be empty");
	
	fttl=attl;
	fm=f_memcached(options_string.cstr(), options_string.length());
	if (connect)
		check("connect", fm, f_memcached_version(fm), MEMCACHED_NOT_SUPPORTED);
}

void VMemcached::open_parse(const String& connect_string, time_t attl){
	load_memcached(memcached_library);
	
	if(connect_string.is_empty())
		throw Exception("memcached", 0, "connect string must not be empty");
	
	fttl=attl;
	fm=f_memcached_create(NULL);
	memcached_server_st* fservers = f_memcached_servers_parse(connect_string.cstr());
	check("server_push", fm, f_memcached_server_push(fm, fservers));
	check("connect", fm, f_memcached_version(fm), MEMCACHED_NOT_SUPPORTED);
}

void VMemcached::flush(time_t attl) {
	check("flush", fm, f_memcached_flush(fm, attl));
}

void VMemcached::quit() {
	f_memcached_quit(fm);
}

void VMemcached::remove(const String& aname) {
	check_key(aname);
	check("delete", fm, f_memcached_delete(fm, aname.cstr(), aname.length(), (time_t)0), MEMCACHED_NOTFOUND);
}

Value* VMemcached::get_element(const String& aname) {
	if(Value *result=VStateless_object::get_element(aname))
		return result;

	check_key(aname);

	memcached_return rc;
	Serialization_data data;
	data.ptr=f_memcached_get(fm, aname.cstr(), aname.length(), &data.length, &data.flags, &rc);

	if(rc==MEMCACHED_SUCCESS){
		return &deserialize(data);
	}
	
	if(rc==MEMCACHED_NOTFOUND)
		return VVoid::get();

	error("get", fm, rc);
	return 0; // calm down compiler
}

Value &VMemcached::mget(ArrayString& akeys) {
	VHash &hresult = *new VHash();
	
	size_t kl = akeys.count();
	
	if(kl==0)
		return hresult;
	
	const char **keys = new(PointerGC) const char *[kl];
	size_t *key_lengths = new(PointerFreeGC) size_t[kl];
	
	for(size_t i=0; i<kl; i++){
		const String &skey = *(akeys[i]);
		check_key(skey);
		keys[i] = skey.cstr();
		key_lengths[i] = skey.length();
	}
	
	check("mget", fm, f_memcached_mget(fm, keys, key_lengths, kl));
	
	// memcached_fetch_result calls memcached_result_create and memcached_result_free, we don't need to do this.
	memcached_result_st *results=0;
	memcached_return rc;
	
	while((results=f_memcached_fetch_result(fm, results, &rc)) && (rc == MEMCACHED_SUCCESS)){
		const char *hkey = pa_strdup(f_memcached_result_key_value(results), f_memcached_result_key_length(results));
		Serialization_data data(f_memcached_result_flags(results), f_memcached_result_value(results), f_memcached_result_length(results));

		hresult.hash().put(hkey, &deserialize(data));
	}

	if (rc != MEMCACHED_END && rc != MEMCACHED_NOTFOUND)
		error("mget", fm, rc);
	
	operator delete[](keys);
	operator delete[](key_lengths);
	
	return hresult;
}

static inline time_t serialize_value(time_t ttl, const String& aname, Value* avalue, Serialization_data &data){

	if(HashStringValue* hash=avalue->get_hash()) {
		int valid_options=1;
		if(Value* ttl_value=hash->get(expires_name)){
			ttl=ttl_value->as_int();
			valid_options++;
		}
		if(avalue=hash->get(value_name)){
			if(avalue->get_junction())
				throw Exception("memcached", 0, VALUE_NAME " must not be code");
		} else
			throw Exception("memcached", &aname, "value hash must contain ." VALUE_NAME);
			
		if(valid_options!=hash->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}

	if(avalue->is_string()){
		serialize_string(*avalue->get_string(), data);
	} else {
		throw Exception("memcached", &aname, "%s serialization is not supported yet", avalue->type());
	}

	return ttl;
}

bool VMemcached::add(const String& aname, Value* avalue){
	check_key(aname);

	Serialization_data data;
	time_t ttl=serialize_value(fttl, aname, avalue, data);

	memcached_return rc=f_memcached_add(fm, aname.cstr(), aname.length(), data.ptr, data.length, ttl, data.flags);

	if(rc == MEMCACHED_NOTSTORED)
		return false;

	if(rc != MEMCACHED_SUCCESS)
		error("add", fm, rc);

	return true;
}

const VJunction* VMemcached::put_element(const String& aname, Value* avalue){
	check_key(aname);

	Serialization_data data;
	time_t ttl=serialize_value(fttl, aname, avalue, data);

	check("set", fm, f_memcached_set(fm, aname.cstr(), aname.length(), data.ptr, data.length, ttl, data.flags));

	return 0;
}

