/*
  $Id: pa_cframe.C,v 1.2 2001/03/08 13:13:41 paf Exp $
*/

#include "pa_vcframe.h"

// intercepting text writes 
void VCodeFrame::write(String& astring) {
	string_wcontext.write(astring);
}

