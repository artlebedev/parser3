/** @file
	Parser: static symbols cache implementation.
	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_symbols.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_SYMBOLS_C="$Id: pa_symbols.C,v 1.2 2016/05/24 12:36:57 moko Exp $" IDENT_PA_SYMBOLS_H;

const String *Symbols::result=Symbols::instance().add("result");
const String *Symbols::caller=Symbols::instance().add("caller");
const String *Symbols::self=Symbols::instance().add("self");

void Symbols::add(const String &astring){
	put_dont_replace(astring,new VString(astring));
}

const String *Symbols::add(const char *astring){
	const String *result=new String(astring);
	put(*result,new VString(*result));
	return result;
}

Symbols &Symbols::instance(){
    static Symbols singleton;
    return singleton;
}
