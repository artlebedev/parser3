/** @file
	Parser: Charset connection decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_charset.h,v 1.4 2001/12/28 18:12:30 paf Exp $
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

#ifdef XML
#include "libxml/encoding.h"
#endif

// defines

#ifdef XML
/*#	include <util/XercesDefs.hpp>
#	include <util/TransService.hpp>
#	define Charset_TransRec XMLTransService::TransRec
*/

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

public:

	struct Tables {
		XMLCh fromTable[0x100];
		Charset_TransRec *toTable;
		uint toTableSize;
	};

private:

	const String& fname;
	bool fisUTF8;
	Tables tables;

#ifdef XML

private:
	void addEncoding(char *name_cstr);
	void initTranscoder(const String *source, const char *name_cstr);

public:
	/// converts GdomeDOMString string to char *
	const char *transcode_cstr(GdomeDOMString *s);
	/// converts GdomeDOMString string to parser String
	String& transcode(GdomeDOMString *s);
	/// converts char * to GdomeDOMString
	GdomeDOMString *transcode_buf(const char *buf, size_t buf_size);
	/// converts parser String to GdomeDOMString
	GdomeDOMString *transcode(const String& s);

private:

	xmlCharEncodingHandler *transcoder;

#endif

};

#endif
