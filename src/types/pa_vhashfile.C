/** @file
	Parser: @b table class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT="$Date: 2003/11/06 10:25:40 $";

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_threads.h"
#include "pa_globals.h"

// methods

void check(const char *step, apr_status_t status) {
	if(status==APR_SUCCESS)
		return;

	throw Exception("todo.todo",
		0,
		"%s error: %d", step, status);
}

void VHashfile::open(const String& afile_name) {
	check("apr_sdbm_open", apr_sdbm_open(&db, afile_name.cstr(String::L_FILE_SPEC), 
                                        APR_WRITE|APR_CREATE|APR_SHARELOCK, 
                                        0664, 0));
}

VHashfile::~VHashfile() {
	if(db)
		check("apr_sdbm_close", apr_sdbm_close(db));
}

void VHashfile::clear() {
}

void VHashfile::put_field(const String& aname, Value *avalue) {
	time_t time_to_die=0;
	const String *value_string;

	if(HashStringValue *hash=avalue->get_hash()) {
		if(Value *value_value=hash->get(value_name)) {
			if(value_value->get_junction())
				throw Exception(0,
					0,
					VALUE_NAME" must not be code");

			value_string=&value_value->as_string();

			if(Value *expires_value=hash->get(expires_name))
				time_to_die=time(0)+(time_t)expires_value->as_double();
		} else
			throw Exception(0,
				&aname,
				"put hash value must contain ."VALUE_NAME);
	} else
		value_string=&avalue->as_string();

//	get_table_ptr(&aname)->put(aname, *value_string, time_to_die);

	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	apr_sdbm_datum_t value;
	value.dptr=const_cast<char*>(value_string->cstr());
	value.dsize=value_string->length();

// *           APR_SDBM_INSERT     return an error if the record exists
 	check("apr_sdbm_store", apr_sdbm_store(db, key, value, APR_SDBM_REPLACE));
}

Value *VHashfile::get_field(const String& aname) {
	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	apr_sdbm_datum_t value;

	check("apr_sdbm_fetch", apr_sdbm_fetch(db, &value, key));

	if(value.dptr)
		return new VString(*new String(pa_strdup(value.dptr, value.dsize)));
	else
		return 0;
}

void VHashfile::remove(const String& aname) {
	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	check("apr_sdbm_delete", apr_sdbm_delete(db, key));
}

HashStringValue *VHashfile::get_hash() {
	HashStringValue& result=*new HashStringValue();

	check("apr_sdbm_lock", apr_sdbm_lock(db, APR_FLOCK_SHARED));
	try {
		apr_sdbm_datum_t apkey;
		apr_status_t status;
		if(apr_sdbm_firstkey(db, &apkey)==APR_SUCCESS)
			do {
				apr_sdbm_datum_t apvalue;
				check("apr_sdbm_fetch", apr_sdbm_fetch(db, &apvalue, apkey));

				const char *clkey=pa_strdup(apkey.dptr, apkey.dsize);
				const char *clvalue=pa_strdup(apvalue.dptr, apvalue.dsize);
				result.put(clkey, new VString(*new String(clvalue)));
			} while(apr_sdbm_nextkey(db, &apkey)==APR_SUCCESS);

		return &result;
	} catch(...) {
			check("apr_sdbm_unlock", apr_sdbm_unlock(db));
			rethrow;
	}

	check("apr_sdbm_unlock", apr_sdbm_unlock(db));
}
