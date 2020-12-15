/** @file
	Parser: exception class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

#include "pa_xml_exception.h"
#include "pa_globals.h"
#include "pa_common.h"
#include "pa_charset.h"

volatile const char * IDENT_PA_XML_EXCEPTION_C="$Id: pa_xml_exception.C,v 1.12 2020/12/15 17:10:37 moko Exp $" IDENT_PA_XML_EXCEPTION_H;

// methods

XmlException::XmlException(const String* aproblem_source, const char* aproblem_comment, ...) {
	ftype="xml";
	fproblem_source=aproblem_source;

	fcomment=new(PointerFreeGC) char[MAX_STRING];
	va_list args;
	va_start(args, aproblem_comment);
	vsnprintf((char *)fcomment, MAX_STRING, aproblem_comment, args);
	va_end(args);
}

XmlException::XmlException(const String* aproblem_source, Request& r){
	ftype="xml";
	fproblem_source=aproblem_source;

	if(const char* xml_generic_errors=xmlGenericErrors()){
		fcomment=pa_strdup(xml_generic_errors);
		if(r.charsets.source().isUTF8())
			fcomment=fixUTF8(fcomment);
	} else
		fcomment="-UNKNOWN ERROR-";
}

XmlException::XmlException(){
	ftype="xml";
	fproblem_source=0;

	if(const char* xml_generic_errors=xmlGenericErrors())
		fcomment=pa_strdup(xml_generic_errors);
	else
		fcomment="-UNKNOWN ERROR-";
}

#endif
