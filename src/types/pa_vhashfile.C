/** @file
	Parser: @b table class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT="$Date: 2003/11/06 08:22:50 $";

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
#if LATER
	time_t time_to_die=0;
	const String *value_string;

	if(HashStringValue *hash=avalue->get_hash(&aname)) {
		if(Value *value_value=(Value *)hash->get(*value_name)) {
			if(value_value->get_junction())
				throw Exception(0, 0,
					&value_value->name(),
					"must not be code");

			value_string=&value_value->as_string();

			if(Value *expires_value=(Value *)hash->get(*expires_name))
				time_to_die=time(0)+(time_t)expires_value->as_double();
		} else
			throw Exception(0, 0,
				&aname,
				"put hash value must contain .%s", VALUE_NAME);
	} else
		value_string=&avalue->as_string();

	get_table_ptr(&aname)->put(aname, *value_string, time_to_die);
#endif
}

Value *VHashfile::get_field(const String& aname) {
#if LATER
	if(String *string=get_table_ptr(&aname)->get(pool(), aname, 0))
		return NEW VString(*string);
	else
		return 0;
#endif
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
