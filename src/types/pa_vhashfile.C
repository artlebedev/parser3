/** @file
	Parser: @b table class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vhashfile.C,v 1.9 2001/10/24 10:09:13 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_threads.h"
#include "pa_globals.h"
//#include "pa_reque

// methods

void VHashfile_cleanup(void *vhashfile) {
	//_asm int 3;
	static_cast<VHashfile *>(vhashfile)->cleanup();
}

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

	get_connection(&aname).put(aname, *value_string, time_to_die);
}

Value *VHashfile::get_field(const String& name) {
	if(String *string=get_connection(&name).get(name))
		return NEW VString(*string);
	else
		return 0;
}

Hash *VHashfile::get_hash(const String *source) {
	Hash& result=*NEW Hash(pool());
	DB_Cursor cursor(get_connection(source), source);

	while(true) {
		String *key;
		String *data;
		if(!cursor.get(key, data, DB_NEXT))
			break;

		if(key) // not expired
			result.put(*key, NEW VString(*data));
	}

	return &result;
}

#endif
