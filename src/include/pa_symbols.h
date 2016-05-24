/** @file
	Parser: static symbols cache.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SYMBOLS_H
#define PA_SYMBOLS_H

#define IDENT_PA_SYMBOLS_H "$Id: pa_symbols.h,v 1.3 2016/05/24 15:42:43 moko Exp $"

#include "pa_common.h"

// cache symbols for faster comparation
#define SYMBOLS_CACHING

#ifdef SYMBOLS_CACHING
#define SYMBOLS_EQ(a,b) (&(a)==&(Symbols::b)) // pointer comparation
#else
#define SYMBOLS_EQ(a,b) ((a)==(Symbols::b)) // string comparation
#endif

class Symbols: public HashStringValue{
public:

static const String SELF_SYMBOL;
static const String CALLER_SYMBOL;
static const String RESULT_SYMBOL;

static const String STATIC_SYMBOL;
static const String DYNAMIC_SYMBOL;

static const String LOCALS_SYMBOL;
static const String PARTIAL_SYMBOL;

static const String REM_SYMBOL;

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
