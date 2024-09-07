/**	@file
	Parser: write context class.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_wcontext.h"

volatile const char * IDENT_PA_WCONTEXT_C="$Id: pa_wcontext.C,v 1.43 2024/09/07 16:30:27 moko Exp $" IDENT_PA_WCONTEXT_H;

// appends a fstring to result
void WContext::write(Value& avalue) {
	if(fvalue) { // already have value?
		// must not construct twice
		throw Exception(PARSER_RUNTIME, 0, "%s may not be overwritten with %s, store it to variable instead", fvalue->type(), avalue.type());
	} else 
		fvalue=&avalue;
}


void WContext::detach_junctions() {
	for(Array_iterator<VJunction*> i(junctions); i; )
		i.next()->reattach(fparent);
}
