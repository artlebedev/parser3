/**	@file
	Parser: write context class.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_WCONTEXT_C="$Date: 2003/01/21 15:51:22 $";

#include "pa_wcontext.h"
#include "pa_exception.h"
#include "pa_request.h"

// appends a fstring to result
void WContext::write(Value& avalue, const String* origin) {
	if(fvalue) { // already have value?
		// must not construct twice
		if(origin)
			throw Exception("parser.runtime",
				origin,
				"contains illegal assignment attempt of %s to %s %s, use constructor",
				avalue.type(), 
					fvalue->get_class()->name_cstr(), fvalue->type());
		else		
			throw Exception("parser.runtime",
				fvalue->get_class()?&fvalue->get_class()->name():0,
				"%s may not be overwritten with %s %s, use constructor",
				fvalue->type(), 
					avalue.get_class()->name_cstr(), avalue.type());
	} else 
		fvalue=&avalue;
}


void WContext::detach_junctions() {
	Array_iter i(junctions);
	while(i.has_next())
		static_cast<Junction *>(i.next())->reattach(fparent);
	// someday free junctions
}
