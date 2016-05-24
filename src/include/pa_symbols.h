/** @file
	Parser: static symbols cache.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SYMBOLS_H
#define PA_SYMBOLS_H

#define IDENT_PA_SYMBOLS_H "$Id: pa_symbols.h,v 1.1 2016/05/24 11:55:13 moko Exp $"

#include "pa_common.h"

class Symbols: public HashStringValue{
public:

void add(const String &astring);
const String *add(const char *astring);

static const String *result;
static const String *caller;
static const String *self;

static Symbols &instance();

};

#endif
