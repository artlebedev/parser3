/** @file
	Parser: @b string class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VSTRING_C="$Id: pa_vstring.C,v 1.20 2002/08/01 11:26:57 paf Exp $";

#include "pa_vstring.h"
#include "pa_vfile.h"

VFile *VString::as_vfile(String::Untaint_lang lang,
		bool origins_mode) {
	VFile& result=*NEW VFile(pool());
	const char *cstr=origins_mode?fstring->cstr_debug_origins():fstring->cstr(lang);
	result.set(false/*not tainted*/, cstr, strlen(cstr));
	return &result;
}
