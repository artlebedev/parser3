/** @file
	Parser: exception class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

static const char * const IDENT="$Date: 2003/11/20 16:34:27 $";

#include "pa_xml_exception.h"
#include "pa_globals.h"
#include "pa_common.h"

// methods

XmlException::XmlException(
	const String* aproblem_source, 
	GdomeException& exc) {
	ftype="xml";
	fproblem_source=aproblem_source;

	const char* xml_generic_errors=xmlGenericErrors();
	if(xml_generic_errors || exc) {
		const char* s;
		switch((GdomeExceptionCode)exc) {
		case GDOME_NOEXCEPTION_ERR: s="NOEXCEPTION_ERR"; break;
		case GDOME_INDEX_SIZE_ERR: s="INDEX_SIZE_ERR"; break;
		case GDOME_DOMSTRING_SIZE_ERR: s="DOMSTRING_SIZE_ERR"; break;
		case GDOME_HIERARCHY_REQUEST_ERR: s="HIERARCHY_REQUEST_ERR"; break;
		case GDOME_WRONG_DOCUMENT_ERR: s="WRONG_DOCUMENT_ERR"; break;
		case GDOME_INVALID_CHARACTER_ERR: s="INVALID_CHARACTER_ERR"; break;
		case GDOME_NO_DATA_ALLOWED_ERR: s="NO_DATA_ALLOWED_ERR"; break;
		case GDOME_NO_MODIFICATION_ALLOWED_ERR: s="NO_MODIFICATION_ALLOWED_ERR"; break;
		case GDOME_NOT_FOUND_ERR: s="NOT_FOUND_ERR"; break;
		case GDOME_NOT_SUPPORTED_ERR: s="NOT_SUPPORTED_ERR"; break;
		case GDOME_INUSE_ATTRIBUTE_ERR: s="INUSE_ATTRIBUTE_ERR"; break;
		case GDOME_INVALID_STATE_ERR: s="INVALID_STATE_ERR"; break;
		case GDOME_SYNTAX_ERR: s="SYNTAX_ERR"; break;
		case GDOME_INVALID_MODIFICATION_ERR: s="INVALID_MODIFICATION_ERR"; break;
		case GDOME_NAMESPACE_ERR: s="NAMESPACE_ERR"; break;
		case GDOME_INVALID_ACCESS_ERR: s="INVALID_ACCESS_ERR"; break;
		case GDOME_NULL_POINTER_ERR: s="NULL_POINTER_ERR"; break;
		default: s="<UNKNOWN CODE>"; break;
		}
		
		fcomment=new(PointerFreeGC) char[MAX_STRING];
		const char* xml_error_message;
		const char* xml_error_prefix;
		if(xml_generic_errors) {
			xml_error_prefix="\n";
			xml_error_message=xml_generic_errors;
		} else {
			xml_error_prefix="";
			xml_error_message="";
		}
		if(exc)
			snprintf(fcomment, MAX_STRING, 
				"DOMException %s (%d)."
				"%s%s", 
					s,  // decoded code of exception
					exc, // DOM exception code
					xml_error_prefix, xml_error_message // xml generic messages accumulated
			);
		else // no DOM exception
			snprintf(fcomment, MAX_STRING, 
				"%s", 
					xml_error_message // xml generic messages accumulated
			);
	} else
		fcomment=0;
}
#endif
