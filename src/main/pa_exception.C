/** @file
	Parser: exception class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_EXCEPTION_C="$Date: 2003/07/24 11:31:23 $";

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_sapi.h"
#include "pa_globals.h"


// methods

Exception::Exception(): ftype(0), fproblem_source(0), fcomment(0) {}
Exception::Exception(const Exception& src):
	ftype(src.ftype),
	fproblem_source(src.fproblem_source),
	fcomment(src.fcomment) {
}
Exception& Exception::operator =(const Exception& src) {
	ftype=src.ftype;
	fproblem_source=src.fproblem_source;
	fcomment=src.fcomment;
	return *this;
}

Exception::Exception(const char* atype, 
		     const String* aproblem_source, 
		     const char* comment_fmt, ...) {
	ftype=atype;
	fproblem_source=aproblem_source;

	if(comment_fmt) {
		fcomment=new(PointerFreeGC) char[MAX_STRING];
		va_list args;
		va_start(args, comment_fmt);
		vsnprintf(fcomment, MAX_STRING, comment_fmt, args);
//		_asm int 3;
		va_end(args);
	} else
		fcomment=0;
}

#ifdef XML
Exception::Exception(
	const String* aproblem_source, 
	GdomeException& exc) :
	ftype("xml"),
	fproblem_source(aproblem_source) {

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

const String* Exception::problem_source() const { 
	return fproblem_source && fproblem_source->length()?fproblem_source:0; 
}
