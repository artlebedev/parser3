/** @file
	Parser: string class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vstring.C,v 1.3 2001/04/11 08:13:43 paf Exp $
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

const VFile *VString::as_vfile(String::Untaint_lang lang) const {
	VFile& result=*NEW VFile(pool());
	const char *cstr=fstring.cstr(lang);
	result.set(false/*not tainted*/, cstr, fstring.size());
	return &result;
}