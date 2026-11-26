/** @file
	Parser: sql driver manager implementation.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_charsets.h"
#include "pa_convert_utf.h"

volatile const char * IDENT_PA_CHARSETS_C="$Id: pa_charsets.C,v 1.31 2026/11/26 21:25:03 moko Exp $" IDENT_PA_CHARSETS_H;

// defines for globals

#define CHARSET_UTF8_NAME "UTF-8"
#define CHARSET_UTF16_NAME "UTF-16"

// globals

static String charset_utf8_name(CHARSET_UTF8_NAME);
Charset pa_UTF8_charset(0, charset_utf8_name, 0/*no file=system*/);

static String charset_latin1_name(CHARSET_LATIN1_NAME);
static Charset latin1_charset(0, charset_latin1_name, 0/*no file=system*/);

static String charset_utf16_name(CHARSET_UTF16_NAME);
Charset pa_UTF16_charset(0, charset_utf16_name, 0/*no file=system*/);

Charsets pa_charsets;

// methods

Charsets::Charsets() {
	put(pa_UTF8_charset.NAME(), &pa_UTF8_charset);
	put(latin1_charset.NAME(), &latin1_charset);
	put("US-ASCII", &latin1_charset);
}

Charset& Charsets::get(String::Body ANAME) {
	ANAME=String::Body(str_upper(ANAME.cstr(), ANAME.length()));
	if(Charset* result=HashString<value_type>::get(ANAME))
		return *result;
	else
		throw Exception(PARSER_RUNTIME, new String(ANAME, String::L_TAINTED), "unknown charset");
}

/// the charset of the files to load and save: UTF-16 can't be used as request/response charset
Charset& Charsets::get_file_charset(String::Body ANAME) {
	return strcasecmp(ANAME.cstr(), CHARSET_UTF16_NAME)==0 ? pa_UTF16_charset : get(ANAME);
}

Charset& Charsets::get_direct(const char *ANAME) {
	if(Charset* result=HashString<value_type>::get(ANAME))
		return *result;
	else
		throw Exception(PARSER_RUNTIME, new String(ANAME, String::L_TAINTED), "unknown charset");
}

static void utf16_to_utf8(char *&body, size_t &body_size, bool big_endian){
	size_t count=body_size/2;
	const UTF16* utf16=(const UTF16*)body;
#ifdef PA_LITTLE_ENDIAN
	if(big_endian || (size_t)body % sizeof(UTF16)) // otherwise as is
#endif
	{	// copied byte by byte: the content is not to be altered as file body can be used later
		UTF16* copy=(UTF16*)pa_malloc_atomic(count*sizeof(UTF16)+1);
		const unsigned char* src=(const unsigned char*)body;
		for(size_t i=0; i<count; i++, src+=2)
			copy[i]=big_endian ? src[0]<<8 | src[1] : src[1]<<8 | src[0];
		utf16=copy;
	}

	char* result=(char*)pa_malloc_atomic(count*3+1); // 3 bytes at most per UTF-16 code unit
	const UTF16* utf16_start=utf16;
	UTF8* result_end=(UTF8*)result;
	pa_convertUTF16toUTF8(&utf16_start, utf16+count, &result_end, (UTF8*)result+count*3, lenientConversion);
	*result_end=0;

	body=result;
	body_size=(char*)result_end-result;
}

Charset* Charsets::checkBOM(char *&body,size_t &body_size, Charset* enforced_charset){
	if(!enforced_charset || enforced_charset->isUTF8()){
		if(body_size>=3 && strncmp(body, "\xEF\xBB\xBF", 3)==0){
			// skip UTF-8 signature (BOM code)
			body+=3;
			body_size-=3;
			return &pa_UTF8_charset;
		}
		if(body_size>=2 && (memcmp(body, "\xFF\xFE", 2)==0 || memcmp(body, "\xFE\xFF", 2)==0)){
			// UTF-16 signature: converted to UTF-8
			bool big_endian=body[0]=='\xFE';
			body+=2;
			body_size-=2;
			utf16_to_utf8(body, body_size, big_endian);
			return &pa_UTF8_charset;
		}
		if(enforced_charset==&pa_UTF16_charset){
			// UTF-16 without signature: little endian
			utf16_to_utf8(body, body_size, false);
			return &pa_UTF8_charset;
		}
	}
	return enforced_charset;
}

// little endian only
String::C Charsets::encodeUTF16(const String::C src, bool bom){
	UTF16* result=(UTF16*)pa_malloc_atomic((src.length+1)*sizeof(UTF16)); // the signature and a code unit at most per UTF-8 byte
	UTF16* utf16_end=result;
	if(bom)
		*utf16_end++=0xFEFF;
	const UTF8* src_start=(const UTF8*)src.str;
	int status=pa_convertUTF8toUTF16(&src_start, (const UTF8*)src.str+src.length, &utf16_end, result+src.length+1, lenientConversion);
	if(status!=conversionOK)
		throw Exception(PARSER_RUNTIME, 0, "invalid UTF-8 text can not be saved in UTF-16 (%d)", status);

#ifndef PA_LITTLE_ENDIAN
	for(UTF16* c=result; c<utf16_end; c++) // our own buffer: swapped in place
		*c=(UTF16)(*c<<8 | *c>>8);
#endif
	return String::C((const char*)result, (char*)utf16_end-(char*)result);
}

void Charsets::load_charset(Request_charsets& charsets, String::Body ANAME, const String& afile_spec) {
	ANAME=String::Body(str_upper(ANAME.cstr(), ANAME.length()));
	//we know that charset?
	if(HashString<value_type>::get(ANAME))
		return; // don't load it then

	put(ANAME, new Charset(&charsets, ANAME, &afile_spec));
}
