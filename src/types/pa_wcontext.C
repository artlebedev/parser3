/**	@file
	Parser: write context class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_wcontext.C,v 1.18 2002/04/18 10:51:02 paf Exp $
*/

#include "pa_wcontext.h"
#include "pa_exception.h"
#include "pa_request.h"

// appends a fstring to result
void WContext::write(const String& astring, uchar lang) {
	fstring.append(astring, lang);
}

void WContext::write(Value& avalue) {
	if(fvalue) { // already have value?
		// must not construct twice
		throw Exception("parser.runtime",
			fvalue->get_class()?&fvalue->get_class()->name():0,
			"(%s) may not be overwritten with '%s' (%s), use constructor",
			fvalue->type(), 
				avalue.get_class()->name_cstr(), avalue.type());
	} else 
		fvalue=&avalue;
}

void WContext::write(Value& avalue, const String* origin) {
	if(fvalue) { // already have value?
		// must not construct twice
		if(origin)
			throw Exception("parser.runtime",
				origin,
				"contains illegal assignment attempt of %s to '%s' (%s), use constructor",
				avalue.type(), 
					fvalue->get_class()->name_cstr(), fvalue->type());
		else		
			throw Exception("parser.runtime",
				fvalue->get_class()?&fvalue->get_class()->name():0,
				"(%s) may not be overwritten with '%s' (%s), use constructor",
				fvalue->type(), 
					avalue.get_class()->name_cstr(), avalue.type());
	} else 
		fvalue=&avalue;
}