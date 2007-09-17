/** @file
	Parser: @b env class.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2007/09/17 15:13:45 $";

#include "pa_venv.h"
#include "pa_vstring.h"
#include "pa_version.h"

#define PARSER_VERSION_ELEMENT_NAME "PARSER_VERSION"

static const String parser_version(PARSER_VERSION);

Value* VEnv::get_element(const String& aname, Value& /*aself*/, bool /*looking_up*/) {
	// $env:PARSER_VERSION
	if(aname==PARSER_VERSION_ELEMENT_NAME)
		return new VString(parser_version);

	// getenv
	String& result=*new String;
	if(const char* local_value=SAPI::get_env(finfo, aname.cstr()))
		result.append_help_length(strdup(local_value), 0, String::L_TAINTED);
	return new VString(result);
}
