/** @file
	Parser: xml exception decls.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_XML_EXCEPTION_H
#define PA_XML_EXCEPTION_H

#ifdef XML

static const char * const IDENT_XML_EXCEPTION_H="$Date: 2004/02/11 15:33:15 $";

// includes

#include "pa_exception.h"
#include "gdome.h"

// defines

class XmlException: public Exception {
public:

	XmlException(
		const String* aproblem_source, 
		GdomeException& exc);

};

#endif

#endif
