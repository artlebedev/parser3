/*
  $Id: pa_cframe.C,v 1.1 2001/02/24 11:20:33 paf Exp $
*/

#include "pa_vcframe.h"

// intercepting text writes 
void VCodeFrame::write(String *astring) {
	string_wcontext.write(astring);
}

