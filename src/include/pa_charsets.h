/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSETS_H
#define PA_CHARSETS_H

static const char * const IDENT_CHARSETS_H="$Date: 2004/02/11 15:33:13 $";

#include "pa_hash.h"
#include "pa_charset.h"

/// convention: use UPPERCASE keys
class Charsets: public Hash<const String::Body, Charset*> {
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
