/** @file
	Parser: @b env class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_venv.h"
#include "pa_vstring.h"
#include "pa_vhash.h"
#include "pa_version.h"

volatile const char * IDENT_PA_PA_VENV_C="$Id: pa_venv.C,v 1.22 2020/12/15 17:10:40 moko Exp $" IDENT_PA_VENV_H;

#define PARSER_VERSION_ELEMENT_NAME "PARSER_VERSION"
#define ENV_FIELDS_ELEMENT_NAME "fields"

static const String parser_version(PARSER_VERSION);

Value* VEnv::get_element(const String& aname) {
#ifndef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	// $env:CLASS, $env:CLASS_NAME
	if(Value* result=VStateless_class::get_element(aname))
		return result;
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
	if(const char* value=SAPI::Env::get(finfo, aname.cstr()))
		return new VString(*new String(value, String::L_TAINTED));
	
	return 0;
}

const VJunction* VEnv::put_element(const String& aname, Value* avalue) {
	if(SAPI::Env::set(finfo, aname.cstr(), avalue->as_string().cstr()))
		return 0;

	return Value::put_element(aname, avalue);
}

