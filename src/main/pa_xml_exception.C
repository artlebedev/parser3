/** @file
	Parser: exception class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

static const char * const IDENT="$Date: 2011/05/25 03:58:11 $";

#include "pa_xml_exception.h"
#include "pa_globals.h"
#include "pa_common.h"

// methods

XmlException::XmlException(const String* aproblem_source, const char* aproblem_comment, ...) {
	ftype="xml";
	fproblem_source=aproblem_source;

	if(aproblem_comment) {
		fcomment=new(PointerFreeGC) char[MAX_STRING];
		va_list args;
		va_start(args, aproblem_comment);
		vsnprintf(fcomment, MAX_STRING, aproblem_comment, args);
		va_end(args);
	} else if(const char* xml_generic_errors=xmlGenericErrors())
		fcomment=pa_strdup(xml_generic_errors);
	else
		fcomment="-UNKNOWN ERROR-";
}
#endif
