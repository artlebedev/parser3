/** @file
	Parser: @b table class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vhashfile.C,v 1.3 2001/10/22 16:44:43 parser Exp $
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

Hash *VHashfile::get_hash() {
	return 0;
}

#endif
