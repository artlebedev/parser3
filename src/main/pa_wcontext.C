/*
  $Id: pa_wcontext.C,v 1.3 2001/02/25 13:23:02 paf Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"

// appends a string to result
void WContext::write(String *astring) {
	if(!astring)
		return;
	
	string+=*astring;
}

// if value is VString writes string,
// else writes Value; raises an error if already
void WContext::write(Value *avalue) {
	if(!avalue)
		return;
	
	String *string=avalue->get_string();
	if(string)
		write(string);
	else
		if(fvalue) // already have value?
			THROW(0,0,  // must not construct twice
			&fvalue->name(),
			"value already assigned, use constructor to reassign it");
		else
			fvalue=avalue;
}

