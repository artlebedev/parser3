/*
  $Id: pa_cframe.C,v 1.4 2001/03/10 11:44:42 paf Exp $
*/

#include "pa_vcframe.h"

// intercepting text writes 
void VCodeFrame::write(const String& astring, String::Untaint_lang lang) {
	string.append(astring, lang);
}

