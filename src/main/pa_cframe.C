/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_cframe.C,v 1.5 2001/03/10 16:34:39 paf Exp $
*/

#include "pa_vcframe.h"

// intercepting text writes 
void VCodeFrame::write(const String& astring, String::Untaint_lang lang) {
	string.append(astring, lang);
}

