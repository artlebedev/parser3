/** @file
	Parser: Charset connection decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_charset.h,v 1.15.2.1 2002/06/27 11:56:18 paf Exp $
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

#	ifndef XMLCh 
		typedef unsigned int XMLCh;
#	endif
#	ifndef XMLByte
		typedef unsigned char XMLByte;
#	endif

struct Charset_TransRec {
	XMLCh intCh;
	XMLByte extCh;
};

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

#ifdef XML
	xmlCharEncodingHandler *transcoder(const String *source);
#endif

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
	/// converts xmlChar* null-terminated string to char *
	const char *transcode_cstr(xmlChar *s);
	/// converts xmlChar* null-terminated string to parser String
	String& transcode(xmlChar *s);
	/** converts sized char * to xmlChar*
		@returns xmlChar * which caller should free
	*/
	xmlChar *transcode_buf2xchar(const char *buf, size_t buf_size);
	/// converts char * to GdomeDOMString
	GdomeDOMString_auto_ptr transcode_buf2dom(const char *buf, size_t buf_size);
	/// converts parser String to GdomeDOMString
	GdomeDOMString_auto_ptr transcode(const String& s);

private:

	xmlCharEncodingHandler *ftranscoder;

#endif

};

#ifdef XML
/// Auto-object used to track GdomeDOMString usage
class GdomeDOMString_auto_ptr {
	GdomeDOMString *fstring;
public:
	explicit GdomeDOMString_auto_ptr(gchar *astring) : fstring(gdome_str_mkref(astring)) {}
	explicit GdomeDOMString_auto_ptr(GdomeDOMString *astring) : fstring(astring) {
		// not ref-ing, owning
	}
	~GdomeDOMString_auto_ptr() {
		gdome_str_unref(fstring);
	}
	GdomeDOMString* get() {
		return fstring;
	}
	GdomeDOMString* operator->() {
		return fstring;
	}
	GdomeDOMString& operator*() {
		return *fstring;
	}

	// copying
	GdomeDOMString_auto_ptr(const GdomeDOMString_auto_ptr& src) : fstring(src.fstring) {
		gdome_str_ref(fstring);
	}
	GdomeDOMString_auto_ptr& operator =(const GdomeDOMString_auto_ptr& src) {
		if(this == &src)
			return *this;

		gdome_str_unref(fstring);
		fstring=src.fstring;
		gdome_str_ref(fstring);

		return *this;
	}
};
#endif

#endif
