/** @file
	Parser: Charset connection decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_CHARSET_H
#define PA_CHARSET_H

#define IDENT_PA_CHARSET_H "$Id: pa_charset.h,v 1.56 2020/12/15 17:10:30 moko Exp $"


#include "pa_exception.h"
#include "pa_common.h"
#include "pa_hash.h"
#include "pa_array.h"

#include "pcre.h"
// we are using some pcre_internal.h stuff as well
#include "../lib/pcre/pa_pcre_internal.h"

#ifdef XML
#include "libxml/encoding.h"
#endif

// defines

#define MAX_CHARSETS 10

#define MAX_CHARSET_UNI_CODES 500

#ifndef XMLCh 
	typedef unsigned int XMLCh;
#endif
#ifndef XMLByte
	typedef unsigned char XMLByte;
#endif

// helpers

typedef HashString<String::Body> HashStringString;

/**	charset holds name & transcode tables 
	registers libxml transcoders
*/
class Charset: public PA_Object {
public:

	Charset(Request_charsets* charsets, const String::Body ANAME, const String* afile_spec);
	
	const String::Body NAME() const { return FNAME; }
	const char* NAME_CSTR() const { return FNAME_CSTR; }

	bool isUTF8() const { return fisUTF8; }

	static String::C transcode(const String::C src, const Charset& source_charset, const Charset& dest_charset);
	static String::Body transcode(const String::Body src, const Charset& source_transcoder, const Charset& dest_transcoder);
	static String& transcode(const String& src, const Charset& source_transcoder, const Charset& dest_transcoder);
	static void transcode(ArrayString& src, const Charset& source_transcoder, const Charset& dest_transcoder);
	static void transcode(HashStringString& src, const Charset& source_transcoder, const Charset& dest_transcoder);

	static String::C escape(const String::C src, const Charset& source_charset);
	static String::Body escape(const String::Body src, const Charset& source_charset);
	static String& escape(const String& src, const Charset& source_charset);

	static String::C escape_JSON(const String::C src, const Charset& source_charset);
	static String::Body escape_JSON(const String::Body src, const Charset& source_charset);
	static String& escape_JSON(const String& src, const Charset& source_charset);

	void store_Char(XMLByte*& outPtr, XMLCh src, XMLByte not_found);

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

	static size_t calc_escaped_length_UTF8(XMLByte* src, size_t src_length);
	static size_t calc_escaped_length(const XMLByte* src, size_t src_length, const Charset::Tables& tables);
	static size_t calc_escaped_length(const String::C src, const Charset& source_charset);
	static size_t escape_UTF8(const XMLByte* src, size_t src_length, XMLByte* dest);
	static size_t escape(const XMLByte* src, size_t src_length, XMLByte* dest, const Charset::Tables& tables);

	static size_t calc_JSON_escaped_length_UTF8(XMLByte* src, size_t src_length);
	static size_t calc_JSON_escaped_length(const XMLByte* src, size_t src_length, const Charset::Tables& tables);
	static size_t calc_JSON_escaped_length(const String::C src, const Charset& source_charset);
	static size_t escape_JSON_UTF8(const XMLByte* src, size_t src_length, XMLByte* dest);
	static size_t escape_JSON(const XMLByte* src, size_t src_length, XMLByte* dest, const Charset::Tables& tables);

#ifdef XML

private:
	void addEncoding(char* name_cstr);
	void initTranscoder(const String::Body name, const char* name_cstr);

public:
	/// converts xmlChar* null-terminated string to char* 
	String::C transcode_cstr(const xmlChar* s);
	/// converts xmlChar* null-terminated string to parser String
	const String& transcode(const xmlChar* s);

	/** converts sized char*  to xmlChar*
		@returns xmlChar*  WHICH CALLER SHOULD FREE
	*/
	xmlChar* transcode_buf2xchar(const char* buf, size_t buf_size);
	/// converts parser String to xmlChar*
	xmlChar* transcode(const String& s);
	/// converts parser String::Body to xmlChar*
	xmlChar* transcode(const String::Body s);

private:

	xmlCharEncodingHandler* ftranscoder;

#endif

};


// externs

extern Charset::UTF8CaseTable UTF8CaseToUpper;
extern Charset::UTF8CaseTable UTF8CaseToLower;
void change_case_UTF8(const XMLByte* srcData, size_t srcLen,
					XMLByte* toFill, size_t toFillLen,
					const Charset::UTF8CaseTable& table);
size_t getUTF8BytePos(const XMLByte* srcBegin, const XMLByte* srcEnd, size_t charPos/*position in characters*/);
size_t getUTF8CharPos(const XMLByte* srcBegin, const XMLByte* srcEnd, size_t bytePos/*position in bytes*/);
size_t lengthUTF8(const XMLByte* srcBegin, const XMLByte* srcEnd);
unsigned int lengthUTF8Char(const XMLByte c);

const char *fixUTF8(const char *src);

class UTF8_string_iterator {
	public:
		UTF8_string_iterator(const String& astring): fsrcPtr((XMLByte*)astring.cstr()), fsrcEnd(fsrcPtr + astring.length()) {}
		UTF8_string_iterator(XMLByte* asrcPtr, size_t length): fsrcPtr(asrcPtr), fsrcEnd(fsrcPtr + length) {}

		bool has_next();
		XMLCh next() { return fUTF8Char; }
		XMLByte getFirstByte(){ return ffirstByte; }
		size_t getCharSize(){ return fcharSize; }
	private:
		const XMLByte* fsrcPtr;
		const XMLByte* fsrcEnd;
		size_t fcharSize;
		XMLByte ffirstByte;
		XMLCh fUTF8Char;
};

#endif
