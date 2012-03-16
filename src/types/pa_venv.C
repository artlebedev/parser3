/** @file
	Parser: @b env class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_venv.h"
#include "pa_vstring.h"
#include "pa_version.h"

volatile const char * IDENT_PA_PA_VENV_C="$Id: pa_venv.C,v 1.12 2012/03/16 09:24:17 moko Exp $" IDENT_PA_VENV_H;

#define PARSER_VERSION_ELEMENT_NAME "PARSER_VERSION"

static const String parser_version(PARSER_VERSION);

Value* VEnv::get_element(const String& aname) {
	// $env:CLASS
	if(aname==CLASS_NAME)
		return this;

	// $env:CLASS_NAME
	if(aname==CLASS_NAMETEXT)
		return new VString(env_class_name);

	// $env:PARSER_VERSION
	if(aname==PARSER_VERSION_ELEMENT_NAME)
		return new VString(parser_version);

	// $env:field
	if(const char* value=SAPI::get_env(finfo, aname.cstr())){
		return new VString(*new String(strdup(value, strlen(value)), String::L_TAINTED));
	}
	
	return 0;
}
