/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_CHARSETS_C="$Date: 2005/08/09 08:14:51 $";

#include "pa_charsets.h"

// defines for globals

#define CHARSET_UTF8_NAME "UTF-8"

// globals

Charset UTF8_charset(0, *new String(CHARSET_UTF8_NAME), 0/*no file=system*/);

Charsets charsets;

// methods

Charsets::Charsets() {
	put(UTF8_charset.NAME(), &UTF8_charset);
}

Charset& Charsets::get(const String::Body ANAME) {
	if(Charset* result=Hash<key_type, value_type>::get(ANAME))
		return *result;
	else
		throw Exception("parser.runtime",
			new String(ANAME, String::L_TAINTED),
			"unknown charset");
}

void Charsets::load_charset(Request_charsets& charsets, const String::Body ANAME, const String& afile_spec) {
	//we know that charset?
	if(Hash<key_type, value_type>::get(ANAME)) 
		return; // don't load it then

	put(ANAME, new Charset(&charsets, ANAME, &afile_spec));
}
