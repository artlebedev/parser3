/**	@file
	Parser: write context class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_wcontext.C,v 1.9 2001/09/26 10:32:26 parser Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"
#include "pa_request.h"

// appends a fstring to result
void WContext::write(const String& astring, String::Untaint_lang lang) {
	fstring.append(astring, lang);
}

void WContext::write(Value& avalue) {
	if(fvalue) { // already have value?
		if(avalue.name()==UNNAMED_NAME)
			THROW(0,0,  // must not construct twice
				&fvalue->name(),
				"(%s) may not be overwritten with '%s' (%s), use constructor instead",
				fvalue->type(), avalue.name().cstr(), avalue.type());
		else
			THROW(0,0,  // must not construct twice
				&avalue.name(),
				"(%s) illegal assignment attempt to '%s' (%s), use constructor instead",
				avalue.type(), fvalue->name().cstr(), fvalue->type());
	} else 
		fvalue=&avalue;
}

// if value is VString writes fstring,
// else writes Value; raises an error if already
void WContext::write(Value& avalue, String::Untaint_lang lang) {
	const String *fstring=avalue.get_string();
	if(fstring)
		write(*fstring, lang);
	else
		write(avalue);
}
