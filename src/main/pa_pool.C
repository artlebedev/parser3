/** @file
	Parser: pool class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_pool.C,v 1.23 2001/09/20 14:34:42 parser Exp $"; 

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"

#include <PlatformSupport/DOMStringHelper.hpp>
#include <util/PlatformUtils.hpp>

Pool::Pool(void *astorage) : 
	fstorage(astorage), fcontext(0), ftag(0), fexception(0),
	charset("UTF-8"), transcoder(0) {
	register_cleanup(Pool_cleanup, this);
}

void Pool_cleanup(void *pool) {
	//_asm int 3;
	static_cast<Pool *>(pool)->cleanup();
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
	set_charset(new_charset.cstr());
}
void Pool::set_charset(const char *new_charset) {
	if(charset && strcasecmp(new_charset, charset)!=0) {
		delete transcoder;  transcoder=0;
		charset=new_charset;
	}
}

void Pool::update_transcoder() {
	if(transcoder)
		return;

	XMLTransService::Codes resValue;
	transcoder=XMLPlatformUtils::fgTransService->makeNewTranscoderFor(charset, resValue, 100);
}


const char *Pool::transcode(const XalanDOMString& s) { 
	update_transcoder();

	const unsigned int len=s.size(); // multibyte-char languages not supported for now
	XMLByte* dest=(XMLByte *)malloc((len+1)*sizeof(XMLByte));
	bool error=true;
	try {
		if(transcoder) {
			unsigned int charsEaten;
			unsigned int size=transcoder->transcodeTo(
				s.c_str(), s.length(),
				dest,
				len,
				charsEaten,
				XMLTranscoder::UnRep_Throw
			);
			dest[size]=0;
			error=false;
		}
	} catch(...) {
	}
	if(error) {
		memset(dest, '?', len-1);
		((char *)dest)[len]=0;
	}
	return (const char *)dest;
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
