/** @file
	Parser: @b string class.

	Copyright(c) 2001, 2002 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vstring.C,v 1.15 2002/02/08 07:27:54 paf Exp $
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

VFile *VString::as_vfile(String::Untaint_lang lang,
		bool origins_mode) {
	VFile& result=*NEW VFile(pool());
	const char *cstr=origins_mode?fstring->cstr_debug_origins():fstring->cstr(lang);
	result.set(false/*not tainted*/, cstr, strlen(cstr));
	return &result;
}
