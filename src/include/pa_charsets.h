/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSETS_H
#define PA_CHARSETS_H

static const char* IDENT_CHARSETS_H="$Date: 2003/01/21 15:51:09 $";

#include "pa_hash.h"

class Charsets : public Hash {
public:
	Charsets(Pool& apool);
	~Charsets();

	Charset& get_charset(const String& name);
	void load_charset(const String& request_name, const String& request_file_spec);
};

/// global
extern Charsets *charsets;


#endif
