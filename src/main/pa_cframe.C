/*
  $Id: pa_cframe.C,v 1.3 2001/03/10 11:03:49 paf Exp $
*/

#include "pa_vcframe.h"

// intercepting text writes 
void VCodeFrame::write(String& astring, String::Untaint_lang lang) {
	string.append(astring, lang);
}

