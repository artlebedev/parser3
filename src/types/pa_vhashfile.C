/** @file
	Parser: @b table class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vhashfile.C,v 1.4 2001/10/23 14:43:44 parser Exp $
*/

#include "pa_config_includes.h"
#ifdef HAVE_LIBDB

#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_threads.h"


// methods

void VHashfile_cleanup(void *vhashfile) {
	//_asm int 3;
	static_cast<VHashfile *>(vhashfile)->cleanup();
}

void VHashfile::put_field(const String& name, Value *value) {
	DB_Connection& connection=get_connection(&name);
	connection.put(name, value->as_string());
}

Value *VHashfile::get_field(const String& name) {
	DB_Connection& connection=get_connection(&name);
	if(String *string=connection.get(name))
		return NEW VString(*string);
	else
		return 0;
}

Hash *VHashfile::get_hash(const String *source) {
	Hash& result=*NEW Hash(pool());
	DB_Connection& connection=get_connection(0/*source*/);
	DB_Cursor cursor=connection.cursor(source);

	while(true) {
		String& key=*NEW String(pool());
		String& data=*NEW String(pool());
		if(!cursor.get(key, data, DB_NEXT))
			break;

		result.put(key, NEW VString(data));
	}

	return &result;
}

#endif
