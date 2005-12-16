/** @file
	Parser: exception class.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

static const char * const IDENT="$Date: 2005/12/16 10:15:12 $";

#include "pa_xml_exception.h"
#include "pa_globals.h"
#include "pa_common.h"

// methods

XmlException::XmlException(const String* aproblem_source) {
	ftype="xml";
	fproblem_source=aproblem_source;

	if(const char* xml_generic_errors=xmlGenericErrors()) {
		fcomment=pa_strdup(xml_generic_errors);
	} else
		fcomment="-UNKNOWN ERROR-";
}
#endif
