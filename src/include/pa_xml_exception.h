/** @file
	Parser: xml exception decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_XML_EXCEPTION_H
#define PA_XML_EXCEPTION_H

#ifdef XML

static const char * const IDENT_XML_EXCEPTION_H="$Date: 2005/08/09 08:14:50 $";

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
