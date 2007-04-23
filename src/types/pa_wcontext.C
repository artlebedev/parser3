/**	@file
	Parser: write context class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_WCONTEXT_C="$Date: 2007/04/23 10:30:50 $";

#include "pa_wcontext.h"

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
	Array_iterator<Junction*> i(junctions);
	while(i.has_next())
		i.next()->reattach(fparent);
	// someday free junctions
	// the day has come?
	// no, libgc will do that for us!
}
