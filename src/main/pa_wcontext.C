/*
  $Id: pa_wcontext.C,v 1.7 2001/03/10 11:03:50 paf Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"

// appends a fstring to result
void WContext::write(String& astring, String::Untaint_lang lang) {
	fstring.append(astring, lang);
}

// if value is VString writes fstring,
// else writes Value; raises an error if already
void WContext::write(Value& avalue) {
	String *fstring=avalue.get_string();
	if(fstring)
		write(*fstring, String::Untaint_lang::APPENDED);
	else
		if(fvalue) // already have value?
			THROW(0,0,  // must not construct twice
			&avalue.name(),
			"(%s) illegal assignment attempt to '%s' (%s), use constructor instead",
				avalue.type(), fvalue->name().cstr(), fvalue->type());
		else
			fvalue=&avalue;
}

