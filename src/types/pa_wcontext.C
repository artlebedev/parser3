/**	@file
	Parser: write context class.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_wcontext.h"

volatile const char * IDENT_PA_WCONTEXT_C="$Id: pa_wcontext.C,v 1.37 2015/10/26 01:22:03 moko Exp $" IDENT_PA_WCONTEXT_H;

// appends a fstring to result
void WContext::write(Value& avalue) {
	if(fvalue) { // already have value?
		// must not construct twice
		throw Exception(PARSER_RUNTIME,
			fvalue->get_class()?&fvalue->get_class()->name():0,
			"%s may not be overwritten with %s, store it to variable instead",
				fvalue->type(), 
				avalue.type());
	} else 
		fvalue=&avalue;
}


void WContext::detach_junctions() {
	Array_iterator<VJunction*> i(junctions);
	while(i.has_next())
		i.next()->reattach(fparent);
}
