/** @file
	Parser: Charset connection decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_CHARSET_H
#define PA_CHARSET_H

static const char * const IDENT_CHARSET_H="$Date: 2003/11/20 16:34:24 $";


#include "pa_exception.h"
#include "pa_common.h"
#include "pa_hash.h"
#include "pa_array.h"

// hiding into namespace so to avoid stupid conflict
namespace PCRE {
#include "pcre.h"
#	include "internal.h"
};

#ifdef XML
#include "libxml/encoding.h"
#include "gdome.h"
#endif

// defines

#define MAX_CHARSETS 10

#define MAX_CHARSET_UNI_CODES 500

#	ifndef XMLCh 
		typedef unsigned int XMLCh;
#	endif
#	ifndef XMLByte
		typedef unsigned char XMLByte;
#	endif

// forwards

class GdomeDOMString_auto_ptr;

// helpers

typedef Hash<const String::Body, String::Body> HashStringString;

/**	charset holds name & transcode tables 
	registers libxml transcoders
*/
class Charset: public PA_Object {
public:

	Charset(Request_charsets* charsets, const String::Body ANAME, const String* afile_spec);
	
	const String::Body NAME() const { return FNAME; }

	bool isUTF8() const { return fisUTF8; }

	static String::C transcode(const String::C src,
		const Charset& source_transcoder, 
		const Charset& dest_transcoder
	);

	static String& transcode(const String& src,
		const Charset& source_transcoder, 
		const Charset& dest_transcoder);

	static String::Body transcode(const String::Body src,
		const Charset& source_transcoder, 
		const Charset& dest_transcoder);

	static void transcode(ArrayString& src,
		const Charset& source_transcoder, 
		const Charset& dest_transcoder);

	static void transcode(HashStringString& src,
		const Charset& source_transcoder, 
		const Charset& dest_transcoder);

#ifdef XML
	xmlCharEncodingHandler& transcoder(const String::Body NAME);
#endif

public:

	unsigned char pcre_tables[tables_length];

private:

	void load_definition(Request_charsets& charsets, const String& afile_spec);
	void sort_ToTable();

	const String::C transcodeToUTF8(const String::C src) const;
	const String::C transcodeFromUTF8(const String::C src) const;
	
	const String::C transcodeToCharset(const String::C src,
		const Charset& dest_transcoder) const;

public:

	struct Tables {
		struct Rec {
			XMLCh intCh;
			XMLByte extCh;
		};

		XMLCh fromTable[0x100];
		Rec toTable[MAX_CHARSET_UNI_CODES];
		uint toTableSize;
	};

	struct UTF8CaseTable {
		struct Rec {
			XMLCh from, to;
		};

		uint size;
		Rec* records;
	};

private:

	const String::Body FNAME;
	char* FNAME_CSTR;
	bool fisUTF8;
	Tables tables;

#ifdef XML

private:
	void addEncoding(char* name_cstr);
	void initTranscoder(const String::Body name, const char* name_cstr);
	
public:
	/// converts GdomeDOMString string to char* 
	String::C transcode_cstr(GdomeDOMString* s);
	/// converts GdomeDOMString string to parser String
	const String& transcode(GdomeDOMString* s);
	/// converts xmlChar* null-terminated string to char* 
	String::C transcode_cstr(xmlChar* s);
	/// converts xmlChar* null-terminated string to parser String
	const String& transcode(xmlChar* s);

	/** converts sized char*  to xmlChar*
		@returns xmlChar*  WHICH CALLER SHOULD FREE
	*/
	xmlChar* transcode_buf2xchar(const char* buf, size_t buf_size);
	/// converts char*  to GdomeDOMString
	GdomeDOMString_auto_ptr transcode_buf2dom(const char* buf, size_t buf_size);
	/// converts parser String to GdomeDOMString
	GdomeDOMString_auto_ptr transcode(const String& s);
	/// converts parser String::Body to GdomeDOMString
	GdomeDOMString_auto_ptr transcode(const String::Body s);

private:

	xmlCharEncodingHandler* ftranscoder;

#endif

};


// externs

extern Charset::UTF8CaseTable UTF8CaseToUpper;
extern Charset::UTF8CaseTable UTF8CaseToLower;
void change_case_UTF8(const XMLByte* srcData, XMLByte* toFill, 
					  const Charset::UTF8CaseTable& table);


#ifdef XML
/// Auto-object used to track GdomeDOMString usage
class GdomeDOMString_auto_ptr {
	GdomeDOMString* fstring;
public:
	/// frees astring afterwards!!!
	explicit GdomeDOMString_auto_ptr(xmlChar* astring) : fstring(gdome_str_mkref_xml(astring)) {}
	explicit GdomeDOMString_auto_ptr(GdomeDOMString* astring=0) : fstring(astring) {
		// not ref-ing, owning
	}
	~GdomeDOMString_auto_ptr() {
		if(fstring)
			gdome_str_unref(fstring);
	}
/*	GdomeDOMString* get() {
		return fstring;
	}*/
	GdomeDOMString* use() {
		if(fstring)
			gdome_str_ref(fstring);
		return fstring;
	}
	GdomeDOMString* operator->() {
		return fstring;
	}
/*	GdomeDOMString& operator*() {
		return* fstring;
	}*/

	// copying
	GdomeDOMString_auto_ptr(const GdomeDOMString_auto_ptr& src) : fstring(src.fstring) {
		gdome_str_ref(fstring);
	}
	GdomeDOMString_auto_ptr& operator =(const GdomeDOMString_auto_ptr& src) {
		if(this == &src)
			return* this;

		if(fstring)
			gdome_str_unref(fstring);
		fstring=src.fstring;
		if(fstring)
			gdome_str_ref(fstring);

		return* this;
	}
};
#endif

#endif
