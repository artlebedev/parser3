/** @file
	Parser: Charset connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_charset.h,v 1.2 2001/12/26 08:46:12 paf Exp $
*/

#ifndef PA_CHARSET_H
#define PA_CHARSET_H

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"

// hiding into namespace so to avoid stupid conflict
namespace PCRE {
#include "pcre.h"
#	include "internal.h"
};

// defines

#ifdef XML
#	include <util/XercesDefs.hpp>
#	include <util/TransService.hpp>
#	define Charset_TransRec XMLTransService::TransRec
#else
#	ifndef XMLCh 
#		define XMLCh unsigned int
#	endif
#	ifndef XMLByte
#		define XMLByte unsigned char
#	endif

	struct Charset_TransRec {
		XMLCh intCh;
		XMLByte extCh;
	};
#endif

/**	charset holds name & transcode tables 
	registers Xerces transcoders
*/
class Charset : public Pooled {
public:

	Charset(Pool& apool, const String& aname, const String *request_file_spec);
	Charset::~Charset();
	
	const String& name() const { return fname; }

	bool isUTF8() const { return fisUTF8; }

	static void transcode(Pool& pool,
		const Charset& source_transcoder, const void *source_body, size_t source_content_length,
		const Charset& dest_transcoder, const void *& dest_body, size_t& dest_content_length
	);

public:

	unsigned char pcre_tables[tables_length];

private:

	void loadDefinition(const String& request_file_spec);
	void sort_ToTable();

	XMLByte xlatOneTo(const XMLCh toXlat) const;
	void transcodeToUTF8(Pool& pool,
									 const void *source_body, size_t source_content_length,
									 const void *& dest_body, size_t& dest_content_length) const;
	void transcodeFromUTF8(Pool& pool,
									   const void *source_body, size_t source_content_length,
									   const void *& dest_body, size_t& dest_content_length) const;
	void transcodeToCharset(Pool& pool,
										   const Charset& dest_transcoder,
										   const void *source_body, size_t source_content_length,
										   const void *& dest_body, size_t& dest_content_length) const;

private:

	const String& fname;
	bool fisUTF8;
	XMLCh fromTable[0x100];
	Charset_TransRec *toTable;
	uint toTableSize;

#ifdef XML

private:
	void addEncoding(const char *name_cstr);
	void initTranscoder(const String *source, const char *name_cstr);

public:
	/// converts Xalan string to char *
	const char *transcode_cstr(const XalanDOMString& s);
	/// converts Xalan string to parser String
	String& transcode(const XalanDOMString& s);
	/// converts char * to Xalan string
	std::auto_ptr<XalanDOMString> transcode_buf(const char *buf, size_t buf_size);
	/// converts parser String to Xalan string
	std::auto_ptr<XalanDOMString> transcode(const String& s);

private:

	XMLTranscoder *transcoder;

#endif

};

#endif
