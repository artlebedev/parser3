/** @file
	Parser: @b env class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_venv.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_version.h"

volatile const char * IDENT_PA_PA_VENV_C="$Id: pa_venv.C,v 1.15 2015/04/02 22:04:41 moko Exp $" IDENT_PA_VENV_H;

#define PARSER_VERSION_ELEMENT_NAME "PARSER_VERSION"
#define ENV_FIELDS_ELEMENT_NAME "fields"

static const String parser_version(PARSER_VERSION);

Value* VEnv::get_element(const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// $env:CLASS
	if(aname==CLASS_NAME)
		return this;

	// $env:CLASS_NAME
	if(aname==CLASS_NAMETEXT)
		return new VString(env_class_name);
#endif

	// $env:PARSER_VERSION
	if(aname==PARSER_VERSION_ELEMENT_NAME)
		return new VString(parser_version);

	// $env:fields
	if(aname==ENV_FIELDS_ELEMENT_NAME){
		HashStringValue *result=new HashStringValue();
		for(SAPI::Env::Iterator i(finfo); i; i.next() )
			result->put(
				i.key(),
				new VString(*new String(i.value(), String::L_TAINTED))
			);
		return new VHash(*result);
	}

	// $env:field
	if(const char* value=SAPI::Env::get(finfo, aname.cstr())){
		return new VString(*new String(value, String::L_TAINTED));
	}
	
	return 0;
}
