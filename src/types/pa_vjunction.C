/**	@file
	Parser: @b junction class impl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VJUNCTION_C="$Date: 2002/10/15 07:44:45 $";

#include "pa_vjunction.h"
#include "pa_wcontext.h"

Value *VJunction::get_element(const String& aname, Value *aself, bool looking_up) { 
	if(Value *rcontext=fjunction.rcontext)
		return rcontext->get_element(aname, aself, looking_up); 
	else
		throw Exception("parser.runtime",
			&aname,
			"can not read from junction without context");
}

bool VJunction::put_element(const String& aname, Value *avalue, bool replace) { 
	if(WContext *wcontext=fjunction.wcontext)
		return wcontext->put_element(aname, avalue, replace); 
	else
		throw Exception("parser.runtime",
			&aname,
			"can not write to junction without context");
}
