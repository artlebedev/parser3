/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.34 2001/10/19 12:43:30 parser Exp $
*/

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"

#ifdef XML
#include <util/PlatformUtils.hpp>
#endif

Pool::Pool(void *astorage) : 
	fstorage(astorage), fcontext(0), ftag(0)
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
	throw Exception(0, 0,
		0,
		"failed to allocate %u bytes", size);
}

void Pool::fail_register_cleanup() const {
	throw Exception(0, 0,
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
	} catch(XMLException& e) {
		exception()._throw(*this, 0, e);
	}
	return (const char *)dest;
}
String& Pool::transcode(const XalanDOMString& s) { 
	return *new(*this) String(*this, transcode_cstr(s)); 
}

/// @test who would free up result?
XalanDOMString& Pool::transcode_buf(const char *buf, size_t buf_size) { 
	update_transcoder();

	unsigned int dest_size=0;
	XMLCh* dest=(XMLCh *)malloc((buf_size+1)*sizeof(XMLCh));
	unsigned char *charSizes=(unsigned char *)malloc(buf_size*sizeof(unsigned char));
	XalanDOMString *result;
	try {
		if(transcoder) {
			unsigned int bytesEaten;
			unsigned int dest_size=transcoder->transcodeFrom(
				(unsigned char *)buf,
				(const unsigned int)buf_size,
				dest, (const unsigned int)buf_size,
				bytesEaten,
				charSizes
			);
			result=new XalanDOMString(dest, dest_size);
		}
	} catch(XMLException& e) {
		exception()._throw(*this, 0, e);
	}
	
	return *result;
}
XalanDOMString& Pool::transcode(const String& s) { 
	const char *cstr=s.cstr(String::UL_XML);

	return transcode_buf(cstr, strlen(cstr)); 
}

#endif
