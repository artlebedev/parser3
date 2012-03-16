/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_charsets.h"

volatile const char * IDENT_PA_CHARSETS_C="$Id: pa_charsets.C,v 1.19 2012/03/16 09:24:13 moko Exp $" IDENT_PA_CHARSETS_H;

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
	if(Charset* result=HashString<value_type>::get(ANAME))
		return *result;
	else
		throw Exception(PARSER_RUNTIME,
			new String(ANAME, String::L_TAINTED),
			"unknown charset");
}

void Charsets::load_charset(Request_charsets& charsets, const String::Body ANAME, const String& afile_spec) {
	//we know that charset?
	if(HashString<value_type>::get(ANAME))
		return; // don't load it then

	put(ANAME, new Charset(&charsets, ANAME, &afile_spec));
}
