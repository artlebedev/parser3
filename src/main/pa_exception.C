/** @file
	Parser: exception class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_exception.C,v 1.37 2002/02/08 07:27:47 paf Exp $
*/

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_globals.h"

Exception::Exception() {
	ftype=fcode=fproblem_source=0;
	owns_comment=false; fcomment=0;
}
Exception::Exception(const String *atype, const String *acode,
					  const String *aproblem_source, 
					  const char *comment_fmt, ...) {
	//_asm int 3;
//__asm__("int3");
	ftype=atype;
	fcode=acode;
	fproblem_source=aproblem_source;
	owns_comment=true;

	if(comment_fmt) {
		fcomment=(char *)malloc(MAX_STRING);
		va_list args;
		va_start(args, comment_fmt);
		vsnprintf(fcomment, MAX_STRING, comment_fmt, args);
		va_end(args);
	} else
		fcomment=0;
}
Exception::Exception(const Exception& src) : 
	ftype(src.ftype),
	fcode(src.fcode),
	fproblem_source(src.fproblem_source),
	fcomment(src.fcomment),
	owns_comment(src.owns_comment) {
	// that ugly string got from STL, along with principal ideal
	const_cast<Exception *>(&src)->owns_comment=false;
}
Exception& Exception::operator =(const Exception& src) {
	ftype=src.ftype;
	fcode=src.fcode;
	fproblem_source=src.fproblem_source;

	if(owns_comment)
		free(fcomment);
	fcomment=src.fcomment;
	// that ugly string got from STL, along with principal ideal
	owns_comment=src.owns_comment;  ((Exception*)&src)->owns_comment=false;
	
	return *this;
}
Exception::~Exception() {
	if(owns_comment)
		free(fcomment);
}

#ifdef XML
Exception::Exception(
	const String *atype, const String *acode,
	const String *aproblem_source, 
	GdomeException& exc) :
	ftype(atype),
	fcode(acode),
	fproblem_source(aproblem_source),
	owns_comment(true) {

	const char *xml_generic_errors=xmlGenericErrors();
	if(xml_generic_errors || exc) {
		const char *s;
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
		
		fcomment=(char *)malloc(MAX_STRING);
		const char *xml_error_message;
		const char *xml_error_prefix;
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
		if(xml_generic_errors)
			free((void *)xml_generic_errors);
	} else
		fcomment=0;
}
#endif
