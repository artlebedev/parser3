/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_charsets.C,v 1.6 2002/02/08 08:30:15 paf Exp $
*/

#include "pa_charsets.h"
#include "pa_charset.h"

// globals

Charsets *charsets;

//

Charsets::Charsets(Pool& apool) : Hash(apool) {
}

static void destroy_charset(const Hash::Key& , Hash::Val *& value, void *) {
	static_cast<Charset *>(value)->~Charset();	
}

Charsets::~Charsets() {
	for_each(destroy_charset);
}

Charset& Charsets::get_charset(const String& name) {
	if(Charset *result=(Charset *)get(name))
		return *result;
	else
		throw Exception(0, 0,
			&name,
			"unknown charset");
}

void Charsets::load_charset(const String& request_name, const String& request_file_spec) {
	//we know that charset?
	if(get(request_name)) 
		return; // don't load it then

	const char *name_cstr=request_name.cstr();
	char *global_name_cstr=(char *)malloc(strlen(name_cstr)+1);
	strcpy(global_name_cstr, name_cstr);
	// make global_name string on global pool
	String& global_name=*NEW String(pool(), global_name_cstr);
	
	put(request_name, NEW Charset(pool(), global_name, &request_file_spec));
}
