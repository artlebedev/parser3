/** @file
	Parser: exception class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

#include "pa_xml_exception.h"
#include "pa_globals.h"
#include "pa_common.h"

volatile const char * IDENT_PA_XML_EXCEPTION_C="$Id: pa_xml_exception.C,v 1.7 2012/03/16 09:24:14 moko Exp $" IDENT_PA_XML_EXCEPTION_H;

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
