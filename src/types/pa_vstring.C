/** @file
	Parser: @b string class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vstring.C,v 1.9 2001/07/18 10:06:04 parser Exp $
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

VFile *VString::as_vfile(String::Untaint_lang lang,
		bool origins_mode) {
	VFile& result=*NEW VFile(pool());
	fstring.set_origins_mode(origins_mode);
	const char *cstr=fstring.cstr(lang);
	result.set(false/*not tainted*/, cstr, strlen(cstr));
	return &result;
}
