/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSETS_H
#define PA_CHARSETS_H

#define IDENT_PA_CHARSETS_H "$Id: pa_charsets.h,v 1.15 2012/03/16 09:24:09 moko Exp $"

#include "pa_hash.h"
#include "pa_charset.h"

/// convention: use UPPERCASE keys
class Charsets: public HashString<Charset*> {
public:

	Charsets();

	Charset& get(const String::Body ANAME);

	void load_charset(Request_charsets& charsets, const String::Body ANAME, const String& afile_spec);
};

//@{ globals
extern Charset UTF8_charset;
extern Charsets charsets;
//@}

#endif
