/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_charsets.C,v 1.2 2001/12/17 18:44:51 paf Exp $
*/

#include "pa_charsets.h"
#include "pa_charset.h"

// globals

Charsets *charsets;

//

Charsets::Charsets(Pool& apool) : Hash(apool) {
}

static void destroy_charset(const Hash::Key& key, Hash::Val *& value, void *) {
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
