/** @file
	Parser: @b table class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT="$Date: 2003/11/10 06:15:10 $";

#include "pa_globals.h"
#include "pa_threads.h"
#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_vdate.h"

// consts

const int HASHFILE_VALUE_SERIALIZED_VERSION=0x0001;

// methods

void check(const char *step, apr_status_t status) {
	if(status==APR_SUCCESS)
		return;

	throw Exception("file.access",
		0,
		"%s error: %s (%d)", 
			step, strerror(status), status);
}

void VHashfile::open(const String& afile_name) {
	check("apr_sdbm_open(shared)", apr_sdbm_open(&db, file_name=afile_name.cstr(String::L_FILE_SPEC), 
                                        APR_CREATE|APR_READ|APR_SHARELOCK, 
                                        0664, 0));
}

void VHashfile::make_writable() {
	if(db && apr_sdbm_rdonly(db)) {
		check("apr_sdbm_close", apr_sdbm_close(db));
		check("apr_sdbm_open(exclusive)", apr_sdbm_open(&db, file_name, 
											APR_WRITE, 
											0664, 0));
	}
}

VHashfile::~VHashfile() {
	if(db)
		check("apr_sdbm_close", apr_sdbm_close(db));
}

struct Hashfile_value_serialized_prolog {
	int version;
	time_t time_to_die;
};

static apr_sdbm_datum_t serialize_value(const String& string, time_t time_to_die) {
	apr_sdbm_datum_t result;

	size_t length=string.length();
	result.dsize=sizeof(Hashfile_value_serialized_prolog)+length;
	result.dptr=new(PointerFreeGC) char[result.dsize];

	Hashfile_value_serialized_prolog& prolog=*reinterpret_cast<Hashfile_value_serialized_prolog*>(result.dptr);
	char *output_cstr=result.dptr+sizeof(Hashfile_value_serialized_prolog);

	prolog.version=HASHFILE_VALUE_SERIALIZED_VERSION;
	prolog.time_to_die=time_to_die;
	memcpy(output_cstr, string.cstr(), length);

	return result;
}

static const String* deserialize_value(const apr_sdbm_datum_t datum) {
	if(!datum.dptr || datum.dsize<sizeof(Hashfile_value_serialized_prolog))
		return 0;

	Hashfile_value_serialized_prolog& prolog=*reinterpret_cast<Hashfile_value_serialized_prolog*>(datum.dptr);
	if(prolog.version!=HASHFILE_VALUE_SERIALIZED_VERSION)
		return 0;
	
	if(prolog.time_to_die/*specified*/ 
		&& (prolog.time_to_die <= time(0)/*expired*/))
		return 0;
	
	char *input_cstr=datum.dptr+sizeof(Hashfile_value_serialized_prolog);
	size_t input_length=datum.dsize-sizeof(Hashfile_value_serialized_prolog);

	return new String(pa_strdup(input_length?input_cstr:0, input_length), true);
}

void VHashfile::put_field(const String& aname, Value *avalue) {
	make_writable();

	time_t time_to_die=0;
	const String *value_string;

	if(HashStringValue *hash=avalue->get_hash()) {
		if(Value *value_value=hash->get(value_name)) {
			if(value_value->get_junction())
				throw Exception(0,
					0,
					VALUE_NAME" must not be code");

			value_string=&value_value->as_string();

			if(Value *expires=hash->get(expires_name)) {
				if(Value* vdate=expires->as(VDATE_TYPE, false))
					time_to_die=static_cast<VDate*>(vdate)->get_time(); // $expires[DATE]
				else if(double days_till_expire=expires->as_double())
					time_to_die=time(NULL)+(time_t)(60*60*24*days_till_expire); // $expires(days)
			}
		} else
			throw Exception(0,
				&aname,
				"put hash value must contain ."VALUE_NAME);
	} else
		value_string=&avalue->as_string();

	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	apr_sdbm_datum_t value=serialize_value(*value_string, time_to_die);

 	check("apr_sdbm_store", apr_sdbm_store(db, key, value, APR_SDBM_REPLACE));
}

Value *VHashfile::get_field(const String& aname) {
	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	apr_sdbm_datum_t value;

	check("apr_sdbm_fetch", apr_sdbm_fetch(db, &value, key));

	const String *sresult=deserialize_value(value);
	return sresult? new VString(*sresult): 0;
}

void VHashfile::remove(const String& aname) {
	make_writable();

	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	check("apr_sdbm_delete", apr_sdbm_delete(db, key));
}

void VHashfile::for_each(void callback(apr_sdbm_datum_t, void*), void* info) const {
	Array<apr_sdbm_datum_t> keys;

	// collect keys
	check("apr_sdbm_lock", apr_sdbm_lock(db, APR_FLOCK_SHARED));
	try {
		apr_sdbm_datum_t key;
		if(apr_sdbm_firstkey(db, &key)==APR_SUCCESS)
			do {
				keys+=key;
			} while(apr_sdbm_nextkey(db, &key)==APR_SUCCESS);
	} catch(...) {
			check("apr_sdbm_unlock", apr_sdbm_unlock(db));
			rethrow;
	}
	check("apr_sdbm_unlock", apr_sdbm_unlock(db));

	// iterate them
	keys.for_each(callback, info);
}

#ifndef DOXYGEN
struct For_each_string_callback_info {
	apr_sdbm_t *db;
	void* nested_info;
	void (*nested_callback)(const String::Body, const String&, void*);
};
#endif
static void for_each_string_callback(apr_sdbm_datum_t apkey, void* ainfo) {
	For_each_string_callback_info& info=*static_cast<For_each_string_callback_info *>(ainfo);

	apr_sdbm_datum_t apvalue;
	check("apr_sdbm_fetch", apr_sdbm_fetch(info.db, &apvalue, apkey));

	const char *clkey=pa_strdup(apkey.dptr, apkey.dsize);
	if(const String* svalue=deserialize_value(apvalue))
		info.nested_callback(clkey, *svalue, info.nested_info);
}
void VHashfile::for_each(void callback(const String::Body, const String&, void*), void* ainfo) const {
	For_each_string_callback_info info;
	
	info.db=db;
	info.nested_info=ainfo;
	info.nested_callback=callback;

	for_each(for_each_string_callback, &info);
}

static void clear_delete_key(apr_sdbm_datum_t key, void* adb) {
	check("apr_sdbm_delete", apr_sdbm_delete(static_cast<apr_sdbm_t*>(adb), key));
}
void VHashfile::clear() {
	make_writable();

	for_each(clear_delete_key, db);
}


static void get_hash__put(const String::Body key, const String& value, void* aresult) {
	static_cast<HashStringValue*>(aresult)->put(key, new VString(value));
}
HashStringValue *VHashfile::get_hash() {
	HashStringValue& result=*new HashStringValue();

	for_each(get_hash__put, &result);
	return &result;
}
