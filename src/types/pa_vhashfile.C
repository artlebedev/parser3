/** @file
	Parser: @b table class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT="$Date: 2003/11/06 09:56:17 $";

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
                                        APR_WRITE|APR_CREATE, 
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
	key.dsize=strlen(key.dptr);

	apr_sdbm_datum_t value;
	value.dptr=const_cast<char*>(value_string->cstr());
	value.dsize=strlen(key.dptr);

// *           APR_SDBM_INSERT     return an error if the record exists
 	check("apr_sdbm_store", apr_sdbm_store(db, key, value, APR_SDBM_REPLACE));
}

Value *VHashfile::get_field(const String& aname) {
	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=strlen(key.dptr);

	apr_sdbm_datum_t value;

	check("apr_sdbm_fetch", apr_sdbm_fetch(db, &value, key));

	if(value.dptr)
		return new VString(String(pa_strdup(value.dptr, value.dsize)));
	else
		return 0;
}

HashStringValue *VHashfile::get_hash(const String *source) {
#if LATER
	DB_Cursor cursor(*get_table_ptr(source), source);

	HashStringValue& result=*NEW HashStringValue(pool());

	while(true) {
		String *key;
		String *data;
		if(!cursor.get(pool(), key, data, DB_NEXT))
			break;

		if(!key) 
			continue; // expired

		result.put(*key, NEW VString(*data));
	}

	return &result;
#endif
	return 0;
}
