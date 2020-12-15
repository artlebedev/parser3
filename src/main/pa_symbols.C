/** @file
	Parser: static symbols cache implementation.
	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_symbols.h"
#include "pa_vstring.h"

volatile const char * IDENT_PA_SYMBOLS_C="$Id: pa_symbols.C,v 1.10 2020/12/15 17:10:37 moko Exp $" IDENT_PA_SYMBOLS_H;

const String Symbols::SELF_SYMBOL("self");
const String Symbols::CALLER_SYMBOL("caller");
const String Symbols::RESULT_SYMBOL("result");

const String Symbols::CLASS_SYMBOL("CLASS");
const String Symbols::CLASS_NAME_SYMBOL("CLASS_NAME");

const String Symbols::METHOD_SYMBOL("method");
const String Symbols::NAME_SYMBOL("name");

const String Symbols::STATIC_SYMBOL("static");
const String Symbols::DYNAMIC_SYMBOL("dynamic");

const String Symbols::LOCALS_SYMBOL("locals");
const String Symbols::PARTIAL_SYMBOL("partial");

const String Symbols::REM_SYMBOL("rem");

const String Symbols::FIELDS_SYMBOL("fields");
const String Symbols::_DEFAULT_SYMBOL("_default");

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

	symbols->set(SELF_SYMBOL);
	symbols->set(CALLER_SYMBOL);
	symbols->set(RESULT_SYMBOL);

	symbols->set(CLASS_SYMBOL);
	symbols->set(CLASS_NAME_SYMBOL);

	symbols->set(METHOD_SYMBOL);
	symbols->set(NAME_SYMBOL);

	symbols->set(STATIC_SYMBOL);
	symbols->set(DYNAMIC_SYMBOL);

	symbols->set(LOCALS_SYMBOL);
	symbols->set(PARTIAL_SYMBOL);

	symbols->set(REM_SYMBOL);

	symbols->set(FIELDS_SYMBOL);
	symbols->set(_DEFAULT_SYMBOL);
}

#endif // SYMBOLS_CACHING
