/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSETS_H
#define PA_CHARSETS_H

static const char* IDENT_CHARSETS_H="$Date: 2003/07/24 11:31:21 $";

#include "pa_hash.h"
#include "pa_charset.h"

/// convention: use UPPERCASE keys
class Charsets: public Hash<const StringBody, Charset*> {
public:

	Charsets();

	Charset& get(const StringBody ANAME);

	void load_charset(Request_charsets& charsets, const StringBody ANAME, const String& afile_spec);
};

//@{ globals
extern Charset UTF8_charset;
extern Charsets charsets;
//@}

#endif
