/** @file
	Parser: @b string class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vstring.C,v 1.18 2002/04/19 11:59:44 paf Exp $
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

const String& VString::optimized_string(bool suppressed_until_forced, char** cstr, bool forced) {
	const String& before=string();
	if(forced 
		|| !suppressed_until_forced && before.is_join_chains_profitable()) {
 		const String& after=before.join_chains(pool(), cstr);
		set_string(after);
		return after;
	} else {
		// optimization may be considered unprofitable, 
		// should indicate that we could not make cstr
		if(cstr)
			*cstr=0;

		return before;
	}
}