/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.30 2001/10/05 17:33:50 parser Exp $
*/

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"

#ifdef XML
#include <PlatformSupport/DOMStringHelper.hpp>
#include <util/PlatformUtils.hpp>
#endif

Pool::Pool(void *astorage) : 
	fstorage(astorage), fcontext(0), ftag(0), fexception(0)
#ifdef XML
	, transcoder(0) 
#endif
	{
#ifdef XML
	charset=new(*this) String(*this, "UTF-8");
#else
	charset=new(*this) String(*this, "");
#endif
}

Pool::~Pool() {
#ifdef XML
	delete transcoder;
#endif
}

void Pool::fail_alloc(size_t size) const {
	fexception->_throw(0, 0,
		0,
		"failed to allocate %u bytes", size);
}

void Pool::fail_register_cleanup() const {
	fexception->_throw(0, 0,
		0,
		"failed to register cleanup");
}

void Pool::set_charset(const String &new_charset) {
	if(new_charset!=*charset) {
#ifdef XML
		delete transcoder;  transcoder=0; // flag "we need new transcoder"
#endif
		charset=&new_charset; // for this charset
	}
}

#ifdef XML
void Pool::update_transcoder() {
	if(transcoder)
		return;

	XMLTransService::Codes resValue;
	transcoder=XMLPlatformUtils::fgTransService->makeNewTranscoderFor(charset->cstr(), resValue, 60);
	if(!transcoder)
		THROW(0, 0,
			charset,
			"unsupported encoding");
}

const char *Pool::transcode_cstr(const XalanDOMString& s) { 
	update_transcoder();

	const unsigned int len=s.size()*2;
	XMLByte* dest=(XMLByte *)malloc((len+1)*sizeof(XMLByte));
	bool error=true;
	try {
		if(transcoder) {
			unsigned int charsEaten;
			unsigned int size=transcoder->transcodeTo(
				s.c_str(), s.length(),
				dest, len,
				charsEaten,
				XMLTranscoder::UnRep_RepChar //UnRep_Throw
			);
			dest[size]=0;
			error=false;
		}
	} catch(...) {
	}
	if(error) {
		memset(dest, '?', s.size());
		((char *)dest)[s.size()]=0;
	}
	return (const char *)dest;
}
String& Pool::transcode(const XalanDOMString& s) { 
	return *new(*this) String(*this, transcode_cstr(s)); 
}

void Pool::_throw(const String *source, const XSLException& e) {
	if(e.getURI().empty())
		THROW(0, 0,
			source,
			"%s (%s)",
				transcode_cstr(e.getMessage()),  // message for exception
				transcode_cstr(e.getType()) // type of exception
		);
	else
		THROW(0, 0,
			source,
			"%s (%s). %s(%d:%d)'", 
				transcode_cstr(e.getMessage()),  // message for exception
				transcode_cstr(e.getType()), // type of exception
				
				transcode_cstr(e.getURI()),  // URI for the associated document, if any
				e.getLineNumber(),  // line number, or -1 if unknown
				e.getColumnNumber() // column number, or -1 if unknown
		);
}

void Pool::_throw(const String *source, const SAXException& e) {
	THROW(0, 0,
		source,
		"%s",
			transcode_cstr(XalanDOMString(e.getMessage()))  // message for exception
	);
}
void Pool::_throw(const String *source, const SAXParseException& e) {
	THROW(0, 0,
		source,
		"%s. %s(%d:%d)",
			transcode_cstr(XalanDOMString(e.getMessage())),  // message for exception
			e.getSystemId()?transcode_cstr(XalanDOMString(e.getSystemId())):"block", // file of exception
			e.getLineNumber(), e.getColumnNumber() // line:col
	);
}


void Pool::_throw(const String *source, const XMLException& e) {
	THROW(0, 0,
		source,
		"%s (%s). %s(%d)'", 
			transcode_cstr(XalanDOMString(e.getMessage())),  // message for exception
			transcode_cstr(XalanDOMString((e.getType()))), // type of exception
			
			e.getSrcFile()?e.getSrcFile():"block", // file of exception
			e.getSrcLine()  // line number
			//e.getCode()
	);
}

void Pool::_throw(const String *source, const XalanDOMException& e) {
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
	THROW(0, 0,
		source,
		"XalanDOMException %s (%d)",
			s,  // decoded code of exception
			code // code of exception
	);
}

#endif
