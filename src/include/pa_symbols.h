/** @file
	Parser: static symbols cache.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SYMBOLS_H
#define PA_SYMBOLS_H

#define IDENT_PA_SYMBOLS_H "$Id: pa_symbols.h,v 1.2 2016/05/24 14:28:24 moko Exp $"

#include "pa_common.h"

// cache symbols for faster comparation
#define SYMBOLS_CACHING

#ifdef SYMBOLS_CACHING
#define SYMBOLS_EQ(a,b) (&(a)==&(b)) // pointer comparation
#else
#define SYMBOLS_EQ(a,b) ((a)==(b)) // string comparation
#endif

class Symbols: public HashStringValue{
public:

static const String result;
static const String caller;
static const String self;

#ifdef SYMBOLS_CACHING
void add(const String &astring);
void set(const String &astring);

static Symbols &instance();
static void init();
#endif

};

#ifdef SYMBOLS_CACHING
extern Symbols *symbols;
#endif

#endif
