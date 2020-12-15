/** @file
	Parser: global decls.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#define IDENT_PA_GLOBALS_H "$Id: pa_globals.h,v 1.118 2020/12/15 17:10:31 moko Exp $"

#ifdef XML
#	include "libxml/tree.h"
#endif

class Request;

/// initialize global variables
void pa_globals_init();

/// finalize global variables
void pa_globals_done();

/// for lt_dlinit to be called once
void pa_dlinit();

/// hex_value[c] = hex value of c
extern short hex_value[0x100];

#ifdef XML
/// flag whether there were some xml generic errors on current thread
bool xmlHaveGenericErrors();
/// get xml generic error accumulated for current thread. WARNING: it is up to caller to free up
const char* xmlGenericErrors();
#endif

/// register request for local thread to retrive later with pa_get_request()
void pa_register_thread_request(Request&);
/// retrives request set by pa_set_request function, useful in contextless places [slow]
Request& pa_thread_request();

#endif
