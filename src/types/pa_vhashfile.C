/** @file
	Parser: @b table class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vhashfile.C,v 1.16 2001/12/07 15:24:48 paf Exp $
*/

#include "pa_config_includes.h"
#ifdef DB2

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_threads.h"
#include "pa_globals.h"

// methods

void VHashfile::put_field(const String& aname, Value *avalue) {
	time_t time_to_die=0;
	const String *value_string;

	if(Hash *hash=avalue->get_hash(&aname)) {
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

	get_table_ptr(&aname)->put(current_transaction, aname, *value_string, time_to_die);
}

Value *VHashfile::get_field(const String& aname) {
	if(String *string=get_table_ptr(&aname)->get(current_transaction, pool(), aname, 0))
		return NEW VString(*string);
	else
		return 0;
}

Hash *VHashfile::get_hash(const String *source) {
	DB_Cursor cursor(*get_table_ptr(source), current_transaction, source);

	Hash& result=*NEW Hash(pool());

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
}

#endif
