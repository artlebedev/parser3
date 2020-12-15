/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSETS_H
#define PA_CHARSETS_H

#define IDENT_PA_CHARSETS_H "$Id: pa_charsets.h,v 1.21 2020/12/15 17:10:30 moko Exp $"

#include "pa_hash.h"
#include "pa_charset.h"

/// convention: use UPPERCASE keys
class Charsets: public HashString<Charset*> {
public:

	Charsets();

	Charset& get(String::Body ANAME);
	Charset& get_direct(const char *ANAME);

	void load_charset(Request_charsets& charsets, String::Body ANAME, const String& afile_spec);

	// detects charset if it's not enforced, skips BOM signature if it complies charset
	static Charset* checkBOM(char *&body,size_t &body_size, Charset* enforced_charset);
};

//@{ globals
extern Charset pa_UTF8_charset;
extern Charsets pa_charsets;
//@}

#endif
