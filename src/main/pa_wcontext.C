/*
  $Id: pa_wcontext.C,v 1.6 2001/03/08 15:50:53 paf Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"

// appends a fstring to result
void WContext::write(String& astring) {
	fstring+=astring;
}

// if value is VString writes fstring,
// else writes Value; raises an error if already
void WContext::write(Value& avalue) {
	String *fstring=avalue.get_string();
	if(fstring)
		write(*fstring);
	else
		if(fvalue) // already have value?
			THROW(0,0,  // must not construct twice
			&avalue.name(),
			"(%s) illegal assignment attempt to '%s' (%s), use constructor instead",
				avalue.type(), fvalue->name().cstr(), fvalue->type());
		else
			fvalue=&avalue;
}

