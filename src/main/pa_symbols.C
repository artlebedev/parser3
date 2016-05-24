/** @file
	Parser: static symbols cache implementation.
	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_symbols.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_SYMBOLS_C="$Id: pa_symbols.C,v 1.3 2016/05/24 14:28:24 moko Exp $" IDENT_PA_SYMBOLS_H;

const String Symbols::result("result");
const String Symbols::caller("caller");
const String Symbols::self("self");

#ifdef SYMBOLS_CACHING

Symbols *symbols=0;

void Symbols::add(const String &astring){
	put_dont_replace(astring,new VString(astring));
}

void Symbols::set(const String &astring){
	put(astring,new VString(astring));
}

Symbols &Symbols::instance(){
	static Symbols singleton;
	return singleton;
}

void Symbols::init(){
	symbols=&instance();

	symbols->add(result);
	symbols->add(caller);
	symbols->add(self);
}

#endif // SYMBOLS_CACHING
