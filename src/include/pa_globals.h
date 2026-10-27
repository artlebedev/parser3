/** @file
	Parser: global decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_GLOBALS_H
#define PA_GLOBALS_H

#define IDENT_PA_GLOBALS_H "$Id: pa_globals.h,v 1.122 2026/10/27 23:38:52 moko Exp $"

#include "pa_config_includes.h"

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

/// request of the local thread: set in the Request constructor and cleared in its destructor, can be used in signal handlers
extern THREAD_LOCAL Request* thread_request;
/// retrives thread_request or throws if there is none, useful in contextless places
Request& pa_thread_request();

#endif
