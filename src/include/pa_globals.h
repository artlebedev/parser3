/** @file
	Parser: global decls.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

static const char* IDENT_GLOBALS_H="$Date: 2003/07/24 11:31:21 $";


#include "pa_hash.h"
#include "pa_string.h"

class Dictionary;
class Charset;
class Table;

/// core func
void pa_globals_init();

/// hex_value[c] = hex value of c
extern short hex_value[0x100];

#ifdef XML
/// DOMImplementation
extern GdomeDOMImplementation *domimpl;

/// flag whether there were some xml generic errors on current thread
bool xmlHaveGenericErrors();
/// get xml generic error accumulated for current thread. WARNING: it is up to caller to free up
const char* xmlGenericErrors();
#endif

#endif
