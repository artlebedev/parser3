/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_pool.C,v 1.26 2001/09/21 14:46:09 parser Exp $"; 

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

#ifdef XML
void Pool::set_charset(const String &new_charset) {
	if(new_charset!=*charset) {
		delete transcoder;  transcoder=0; // flag "we need new transcoder"
		charset=&new_charset; // for this charset
	}
}

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
				transcode(e.getMessage()),  // message for exception
				transcode(e.getType()) // type of exception
		);
	else
		THROW(0, 0,
			source,
			"%s (%s) %s(%d:%d)'", 
				transcode(e.getMessage()),  // message for exception
				transcode(e.getType()), // type of exception
				
				transcode(e.getURI()),  // URI for the associated document, if any
				e.getLineNumber(),  // line number, or -1 if unknown
				e.getColumnNumber() // column number, or -1 if unknown
		);
}

#endif
