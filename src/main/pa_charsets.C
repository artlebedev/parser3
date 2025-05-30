/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_charsets.h"

volatile const char * IDENT_PA_CHARSETS_C="$Id: pa_charsets.C,v 1.29 2025/05/26 00:52:15 moko Exp $" IDENT_PA_CHARSETS_H;

// defines for globals

#define CHARSET_UTF8_NAME "UTF-8"

// globals

static String charset_utf8_name(CHARSET_UTF8_NAME);
Charset pa_UTF8_charset(0, charset_utf8_name, 0/*no file=system*/);

Charsets pa_charsets;

// methods

Charsets::Charsets() {
	put(pa_UTF8_charset.NAME(), &pa_UTF8_charset);
}

Charset& Charsets::get(String::Body ANAME) {
	ANAME=String::Body(str_upper(ANAME.cstr(), ANAME.length()));
	if(Charset* result=HashString<value_type>::get(ANAME))
		return *result;
	else
		throw Exception(PARSER_RUNTIME, new String(ANAME, String::L_TAINTED), "unknown charset");
}

Charset& Charsets::get_direct(const char *ANAME) {
	if(Charset* result=HashString<value_type>::get(ANAME))
		return *result;
	else
		throw Exception(PARSER_RUNTIME, new String(ANAME, String::L_TAINTED), "unknown charset");
}

Charset* Charsets::checkBOM(char *&body,size_t &body_size, Charset* enforced_charset){
	if((!enforced_charset || enforced_charset->isUTF8()) && (body_size>=3 && strncmp(body, "\xEF\xBB\xBF", 3)==0)){
		// skip UTF-8 signature (BOM code)
		body+=3;
		body_size-=3;
		return &pa_UTF8_charset;
	}
	return enforced_charset;
}

void Charsets::load_charset(Request_charsets& charsets, String::Body ANAME, const String& afile_spec) {
	ANAME=String::Body(str_upper(ANAME.cstr(), ANAME.length()));
	//we know that charset?
	if(HashString<value_type>::get(ANAME))
		return; // don't load it then

	put(ANAME, new Charset(&charsets, ANAME, &afile_spec));
}
