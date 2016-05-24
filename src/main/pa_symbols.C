/** @file
	Parser: static symbols cache implementation.
	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_symbols.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_SYMBOLS_C="$Id: pa_symbols.C,v 1.5 2016/05/24 16:38:40 moko Exp $" IDENT_PA_SYMBOLS_H;

const String Symbols::SELF_SYMBOL("self");
const String Symbols::CALLER_SYMBOL("caller");
const String Symbols::RESULT_SYMBOL("result");

const String Symbols::CLASS_SYMBOL("CLASS");
const String Symbols::CLASS_NAME_SYMBOL("CLASS_NAME");

const String Symbols::STATIC_SYMBOL("static");
const String Symbols::DYNAMIC_SYMBOL("dynamic");

const String Symbols::LOCALS_SYMBOL("locals");
const String Symbols::PARTIAL_SYMBOL("partial");

const String Symbols::REM_SYMBOL("rem");


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

	symbols->add(SELF_SYMBOL);
	symbols->add(CALLER_SYMBOL);
	symbols->add(RESULT_SYMBOL);

	symbols->add(CLASS_SYMBOL);
	symbols->add(CLASS_NAME_SYMBOL);

	symbols->add(STATIC_SYMBOL);
	symbols->add(DYNAMIC_SYMBOL);

	symbols->add(LOCALS_SYMBOL);
	symbols->add(PARTIAL_SYMBOL);

	symbols->add(REM_SYMBOL);
}

#endif // SYMBOLS_CACHING
