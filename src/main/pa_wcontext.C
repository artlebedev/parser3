/*
  $Id: pa_wcontext.C,v 1.4 2001/03/08 12:19:21 paf Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"

// appends a fstring to result
void WContext::write(String *astring) {
	if(!astring)
		return;
	
	fstring+=*astring;
}

// if value is VString writes fstring,
// else writes Value; raises an error if already
void WContext::write(Value *avalue) {
	if(!avalue)
		return;
	
	String *fstring=avalue->get_string();
	if(fstring)
		write(fstring);
	else
		if(fvalue) // already have value?
			THROW(0,0,  // must not construct twice
			&fvalue->name(),
			"value already assigned, use constructor to reassign it");
		else
			fvalue=avalue;
}

