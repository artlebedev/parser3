/** @file
	Parser: exception class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_exception.C,v 1.27 2001/11/20 17:58:47 paf Exp $
*/

#include "pa_common.h"
#include "pa_exception.h"

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
		va_list args;
		va_start(args, comment_fmt);
		fcomment=(char *)malloc(MAX_STRING);
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
void Exception::provide_source(Pool& pool, const String *source, const XSLException& e) {
	if(e.getURI().empty())
		throw Exception(0, 0,
			source,
			"%s (%s)",
				pool.transcode_cstr(e.getMessage()),  // message for exception
				pool.transcode_cstr(e.getType()) // type of exception
		);
	else
		throw Exception(0, 0,
			source,
			"%s (%s). %s(%d:%d)'", 
				pool.transcode_cstr(e.getMessage()),  // message for exception
				pool.transcode_cstr(e.getType()), // type of exception
				
				pool.transcode_cstr(e.getURI()),  // URI for the associated document, if any
				e.getLineNumber(),  // line number, or -1 if unknown
				e.getColumnNumber() // column number, or -1 if unknown
		);
}

void Exception::provide_source(Pool& pool, const String *source, const SAXException& e) {
	throw Exception(0, 0,
		source,
		"%s",
			pool.transcode_cstr(XalanDOMString(e.getMessage()))  // message for exception
	);
}
void Exception::provide_source(Pool& pool, const String *source, const SAXParseException& e) {
	throw Exception(0, 0,
		source,
		"%s. %s(%d:%d)",
			pool.transcode_cstr(XalanDOMString(e.getMessage())),  // message for exception
			e.getSystemId()?pool.transcode_cstr(XalanDOMString(e.getSystemId())):"block", // file of exception
			e.getLineNumber(), e.getColumnNumber() // line:col
	);
}


void Exception::provide_source(Pool& pool, const String *source, const XMLException& e) {
	throw Exception(0, 0,
		source,
		"%s (%s). %s(%d)'", 
			pool.transcode_cstr(XalanDOMString(e.getMessage())),  // message for exception
			pool.transcode_cstr(XalanDOMString(e.getType())), // type of exception
			
			e.getSrcFile()?e.getSrcFile():"block", // file of exception
			e.getSrcLine()  // line number
			//e.getCode()
	);
}

void Exception::provide_source(Pool& pool, const String *source, const XalanDOMException& e) {
	const char *s;
	int code=(int)e.getExceptionCode();
	switch(code) {
	case 1: s="INDEX_SIZE_ERR"; break;
	case 2: s="DOMSTRING_SIZE_ERR"; break;
	case 3: s="HIERARCHY_REQUEST_ERR"; break;
	case 4: s="WRONG_DOCUMENT_ERR"; break;
	case 5: s="INVALID_CHARACTER_ERR"; break;
	case 6: s="NO_DATA_ALLOWED_ERR"; break;
	case 7: s="NO_MODIFICATION_ALLOWED_ERR"; break;
	case 8: s="NOT_FOUND_ERR"; break;
	case 9: s="NOT_SUPPORTED_ERR"; break;
	case 10: s="INUSE_ATTRIBUTE_ERR"; break;
	case 11: s="INVALID_STATE_ERR"; break;
	case 12: s="SYNTAX_ERR"; break;
	case 13: s="INVALID_MODIFICATION_ERR"; break;
	case 14: s="NAMESPACE_ERR"; break;
	case 15: s="INVALID_ACCESS_ERR"; break;
	case 201: s="UNKNOWN_ERR"; break;
	case 202: s="TRANSCODING_ERR"; break;
	default: s="<UNKNOWN CODE>"; break;
	}
	throw Exception(0, 0,
		source,
		"XalanDOMException %s (%d)",
			s,  // decoded code of exception
			code // code of exception
	);
}
#endif
