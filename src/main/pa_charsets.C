/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_CHARSETS_C="$Id: pa_charsets.C,v 1.9 2002/08/01 11:26:49 paf Exp $";

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
		throw Exception("parser.runtime",
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
	
	put(global_name, NEW Charset(pool(), global_name, &request_file_spec));
}
