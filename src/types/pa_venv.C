/** @file
	Parser: @b env class.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2009/08/08 13:30:21 $";

#include "pa_venv.h"
#include "pa_vstring.h"
#include "pa_version.h"

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
