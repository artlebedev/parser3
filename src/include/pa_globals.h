/** @file
	Parser: global decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

static const char * const IDENT_GLOBALS_H="$Date: 2005/12/16 10:15:12 $";

#ifdef XML
#	include "libxml/tree.h"
#endif

class Request;

/// initialize global variables
void pa_globals_init();

/// finalize global variables
void pa_globals_done();

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
