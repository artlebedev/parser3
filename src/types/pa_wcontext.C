/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_wcontext.C,v 1.1 2001/03/11 07:52:46 paf Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"

// appends a fstring to result
void WContext::write(const String& astring, String::Untaint_lang lang) {
	fstring.append(astring, lang);
}

// if value is VString writes fstring,
// else writes Value; raises an error if already
void WContext::write(Value& avalue, String::Untaint_lang lang) {
	const String *fstring=avalue.get_string();
	if(fstring)
		write(*fstring, lang);
	else
		if(fvalue) // already have value?
			THROW(0,0,  // must not construct twice
			&avalue.name(),
			"(%s) illegal assignment attempt to '%s' (%s), use constructor instead",
				avalue.type(), fvalue->name().cstr(), fvalue->type());
		else
			fvalue=&avalue;
}

