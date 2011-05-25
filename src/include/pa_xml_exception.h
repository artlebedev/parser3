/** @file
	Parser: xml exception decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_XML_EXCEPTION_H
#define PA_XML_EXCEPTION_H

#ifdef XML

static const char * const IDENT_XML_EXCEPTION_H="$Date: 2011/05/25 03:57:46 $";

const char* const XML_VALUE_MUST_BE_STRING = "value must be string";
const char* const XML_NAMESPACEURI_MUST_BE_STRING = "namespaceURI must be string";

const char* const XML_QUALIFIED_NAME_MUST_BE_STRING = "qualifiedName must be string";
const char* const XML_NC_NAME_MUST_BE_STRING = "NCName must be string";
const char* const XML_LOCAL_NAME_MUST_BE_STRING = "localName must be string";

const char* const XML_INVALID_QUALIFIED_NAME = "invalid qualifiedName '%s'";
const char* const XML_INVALID_NC_NAME = "invalid NCName '%s'";
const char* const XML_INVALID_LOCAL_NAME = "invalid localName '%s'";

// includes

#include "pa_exception.h"

// defines

class XmlException: public Exception {
public:

	XmlException(const String* aproblem_source, const char* aproblem_comment=0, ...);
};

#endif

#endif
