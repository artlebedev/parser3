/** @file
	Parser: @b string class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vstring.C,v 1.6 2001/05/21 16:38:46 parser Exp $
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

const VFile *VString::as_vfile(String::Untaint_lang lang) const {
	VFile& result=*NEW VFile(pool());
	const char *cstr=fstring->cstr(lang);
	result.set(false/*not tainted*/, cstr, strlen(cstr));
	return &result;
}