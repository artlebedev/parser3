/** @file
	Parser: commonly functions.

	Copyright (c) 2000-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_inline_hash.h"
#include "pa_globals.h"
#include "pa_charsets.h"
#include "pa_request_charsets.h"
#include "pa_file.h"
#include "pa_stack.h"

#include "pa_idna.h"
#include "pa_convert_utf.h"

volatile const char * IDENT_PA_COMMON_C="$Id: pa_common.C,v 1.349 2026/10/01 11:48:37 moko Exp $" IDENT_PA_COMMON_H IDENT_PA_HASH_H IDENT_PA_INLINE_HASH_H IDENT_PA_ARRAY_H IDENT_PA_STACK_H;

// defines for globals

#define FILE_STATUS_NAME  "status"

// globals

const String file_status_name(FILE_STATUS_NAME);

String sql_bind_name(SQL_BIND_NAME);
String sql_limit_name(PA_SQL_LIMIT_NAME);
String sql_offset_name(PA_SQL_OFFSET_NAME);
String sql_default_name(SQL_DEFAULT_NAME);
String sql_distinct_name(SQL_DISTINCT_NAME);
String sql_value_type_name(SQL_VALUE_TYPE_NAME);

bool capitalized(const char* s){
	bool upper=true;
	for(const char* c=s; *c; c++){
		if(*c != (upper ? toupper((unsigned char)*c) : tolower((unsigned char)*c)))
			return false;
		upper=strchr("-_ ", *c) != 0;
	}
	return true;
}

const char* capitalize(const char* s){
	if(!s || capitalized(s))
		return s;

	char* result=pa_strdup(s);
	if(result){
		bool upper=true;
		for(char* c=result; *c; c++){
			*c=upper ? (char)toupper((unsigned char)*c) : (char)tolower((unsigned char)*c);
			upper=strchr("-_ ", *c) != 0;
		}
	}
	return (const char*)result;
}

char *str_lower(const char *s, size_t length){
	char *result=pa_strdup(s, length);
	for(char* c=result; *c; c++)
		*c=(char)tolower((unsigned char)*c);
	return result;
}

char *str_upper(const char *s, size_t length){
	char *result=pa_strdup(s, length);
	for(char* c=result; *c; c++)
		*c=(char)toupper((unsigned char)*c);
	return result;
}

void fix_line_breaks(char *str, size_t& length) {
	//_asm int 3;
	const char* const eob=str+length;
	char* dest=str;
	// fix DOS: \r\n -> \n
	// fix Macintosh: \r -> \n
	char* bol=str;
	while(char* eol=(char*)memchr(bol, '\r', eob -bol)) {
		size_t len=eol-bol;
		if(dest!=bol)
			memmove(dest, bol, len);
		dest+=len;
		*dest++='\n'; 

		if(&eol[1]<eob && eol[1]=='\n') { // \r, \n = DOS
			bol=eol+2;
			length--; 
		} else // \r, not \n = Macintosh
			bol=eol+1;
	}
	// last piece without \r
	if(dest!=bol)
		memmove(dest, bol, eob-bol); 
	str[length]=0; // terminating
}

/**
	scans for @a delim[default \n] in @a *row_ref,
	@return piece of line before it or end of string, if no @a delim found
	assigns @a *row_ref to point right after delimiter if there were one
	or to zero if no @a delim were found.
*/

char* getrow(char* *row_ref, char delim) {
	char* result=*row_ref;
	if(result) {
		*row_ref=strchr(result, delim); 
		if(*row_ref) 
			*((*row_ref)++)=0; 
		else if(!*result) 
			return 0;
	}
	return result;
}

const char* strrpbrk(const char* string, const char* chars) {
	if(string) {
		const char* pos=string+strlen(string);
		while(pos!=string) {
			if(strchr(chars, *--pos))
				return pos;
		}
	}
	return NULL;
}

char* lsplit(char* string, char delim) {
	if(string) {
		char* v=strchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
	}
	return 0;
}

char* lsplit(char* *string_ref, char delim) {
	char* result=*string_ref;
	char* next=lsplit(*string_ref, delim);
	*string_ref=next;
	return result;
}

char* rsplit(char* string, char delim) {
	if(string) {
		char* v=strrchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
	}
	return NULL;
}

char* rsplit(char* string, const char* delims) {
	if(char* v=strrpbrk(string, delims)) {
		*v=0;
		return v+1;
	}
	return NULL;
}

void pa_strncpy(char *dst, const char *src, size_t dst_size){
	size_t left = dst_size;

	if (left != 0 && src) {
		while (--left != 0) {
			if ((*dst++ = *src++) == '\0')
				return;
		}
	}
	if (dst_size != 0)
		*dst = '\0';
}

#define STRCAT_STEP(str, len) if(str) { memcpy(ptr, str, len); ptr += len; }

char *pa_strcat(const char *a, const char *b, const char *c) {
	size_t len_a = a ? strlen(a) : 0;
	size_t len_b = b ? strlen(b) : 0;
	size_t len_c = c ? strlen(c) : 0;
	char *result=new(PointerFreeGC) char[len_a + len_b + len_c +1/*0*/];
	char *ptr=result;
	STRCAT_STEP(a, len_a);
	STRCAT_STEP(b, len_b);
	STRCAT_STEP(c, len_c);
	*ptr='\0';
	return result;
}


size_t stdout_write(const void *buf, size_t size) {
#ifdef WIN32
	size_t to_write = size;
	do{
		int chunk_written=fwrite(buf, 1, min((size_t)8*0x400, size), stdout);
		if(chunk_written<=0)
			break;
		size-=chunk_written;
		buf=((const char*)buf)+chunk_written;
	} while(size>0);

	fflush(stdout);
	return to_write-size;
#else
	size_t result=fwrite(buf, 1, size, stdout);
	fflush(stdout);
	return result;
#endif
}

enum EscapeState {
	EscapeRest,
	EscapeFirst,
	EscapeSecond,
	EscapeUnicode
};

// @todo prescan for reduce required size (unescaped sting in 1 byte charset requires less memory usually)
char* unescape_chars(const char* cp, int len, Charset* charset, bool js){
	char* s=new(PointerFreeGC) char[len+1]; // must be enough (%uXXXX==6 bytes, max utf-8 char length==6 bytes)
	XMLByte* dst=(XMLByte *)s;
	EscapeState escapeState=EscapeRest;
	uint escapedValue=0;
	int srcPos=0;
	short int jsCnt=0;
	while(srcPos<len){
		uchar c=(uchar)cp[srcPos]; 
		if(c=='%' || (c=='\\' && js)){
			escapeState=EscapeFirst;
		} else {
			switch(escapeState) {
				case EscapeRest:
					if(c=='+' && !js){
						*dst++=' ';
					} else {
						*dst++=c;
					}
					break;
				case EscapeFirst:
					if(charset && c=='u'){
						// escaped unicode value: %u0430
						jsCnt=0;
						escapedValue=0;
						escapeState=EscapeUnicode;
					} else {
						if(isxdigit(c)){
							escapedValue=hex_value[c] << 4;
							escapeState=EscapeSecond;
						} else {
							*dst++=c;
							escapeState=EscapeRest;
						}
					}
					break;
				case EscapeSecond:
					if(isxdigit(c)){
						escapedValue+=hex_value[c]; 
						*dst++=(char)escapedValue;
					}
					escapeState=EscapeRest;
					break;
				case EscapeUnicode:
					if(isxdigit(c)){
						escapedValue=(escapedValue << 4) + hex_value[c];
						if(++jsCnt==4){
							// transcode utf8 char to client charset (we can lost some chars here)
							charset->store_Char(dst, (XMLCh)escapedValue, '?');
							escapeState=EscapeRest;
						}
					} else {
						// not full unicode value
						escapeState=EscapeRest;
					}
					break;
			}
		}

		srcPos++;
	}

	*dst=0; // zero-termination
	return s;
}

char *search_stop(char*& current, char cstop_at) {
	// sanity check
	if(!current)
		return 0;

	// skip leading WS
	while(*current==' ' || *current=='\t')
		current++;
	if(!*current)
		return current=0;

	char *result=current;
	if(char *pstop_at=strchr(current, cstop_at)) {
		*pstop_at=0;
		current=pstop_at+1;
	} else
		current=0;
	return result;
}

char* backslashes_to_slashes(const char* path) {
	if(!path)
		return 0;
	char* result=pa_strdup(path);
	for(char* s=result; *s; s++)
		if(*s=='\\')
			*s='/';
	return result;
}

const String& backslashes_to_slashes(const String& path) {
	return path.pos('\\')==STRING_NOT_FOUND ? path : path.change_char('\\', '/');
}

size_t strpos(const char *str, const char *substr) {
	const char *p = strstr(str, substr);
	return (p==0)?STRING_NOT_FOUND:p-str;
}

size_t remove_crlf(char* start, char* end) {
	char* from=start;
	char* to=start;
	bool skip=false;
	while(from < end){
		switch(*from){
			case '\n':
			case '\r':
			case '\t':
			case ' ':
				if(!skip){
					*to=' ';
					to++;
					skip=true;
				}
				break;
			default:
				if(from != to)
					*to=*from;
				to++;
				skip=false;
		}
		from++;
	}
	return to-start;
}

const char* hex_digits="0123456789ABCDEF";

const char* hex_string(unsigned char* bytes, size_t size, bool upcase) {
	char *bytes_hex=new(PointerFreeGC) char [size*2/*byte->hh*/+1/*for zero-teminator*/];
	unsigned char *src=bytes;
	unsigned char *end=bytes+size;
	char *dest=bytes_hex;

	const char *hex=upcase? hex_digits : "0123456789abcdef";

	for(; src<end; src++) {
		*dest++=hex[*src/0x10];
		*dest++=hex[*src%0x10];
	}
	*dest=0;

	return bytes_hex;
}

ssize_t file_block_read(const int f, void* buffer, const size_t size){
	ssize_t nCount = read(f, buffer, size);
	if (nCount < 0)
		throw Exception("file.read", 0, "read failed: %s (%d)", strerror(errno), errno); 
	return nCount;
}

static unsigned long crc32Table[256];
static void InitCrc32Table()
{
	if(crc32Table[1] == 0){
		// This is the official polynomial used by CRC32 in PKZip.
		// Often times the polynomial shown reversed as 0x04C11DB7.
		static const unsigned long dwPolynomial = 0xEDB88320;

		for(int i = 0; i < 256; i++)
		{
			unsigned long dwCrc = i;
			for(int j = 8; j > 0; j--)
			{
				if(dwCrc & 1)
					dwCrc = (dwCrc >> 1) ^ dwPolynomial;
				else
					dwCrc >>= 1;
			}
			crc32Table[i] = dwCrc;
		}
	}
}

inline void CalcCrc32(const unsigned char byte, unsigned long &crc32)
{
	crc32 = ((crc32) >> 8) ^ crc32Table[(byte) ^ ((crc32) & 0x000000FF)];
}


unsigned long pa_crc32(const char *in, size_t in_size){
	unsigned long crc32=0xFFFFFFFF;

	InitCrc32Table();
	for(size_t i = 0; i<in_size; i++)
		CalcCrc32(in[i], crc32);

	return ~crc32; 
}

static void file_crc32_file_action(struct stat& finfo, int f, const String&, void *context) {
	unsigned long& crc32=*static_cast<unsigned long *>(context);
	if(finfo.st_size) {
		InitCrc32Table();
		int nCount=0;
		do {
			unsigned char buffer[FILE_BUFFER_SIZE];
			nCount = file_block_read(f, buffer, sizeof(buffer));
			for(int i = 0; i < nCount; i++) CalcCrc32(buffer[i], crc32);
		} while(nCount > 0);
	}
}

unsigned long pa_crc32(const String& file_spec){
	unsigned long crc32=0xFFFFFFFF;
	file_read_action_under_lock(file_spec, "crc32", file_crc32_file_action, &crc32);
	return ~crc32; 
}

// content-type: xxx; charset=WE-NEED-THIS
// content-type: xxx; charset="WE-NEED-THIS"
// content-type: xxx; charset="WE-NEED-THIS";
Charset* detect_charset(const char* content_type){
	if(content_type){
		char* CONTENT_TYPE=str_upper(content_type);

		if(const char* begin=strstr(CONTENT_TYPE, "CHARSET=")){
			begin+=8; // skip "CHARSET="
			char* end=0;
			if(*begin && (*begin=='"' || *begin =='\'')){
				char quote=*begin;
				begin++;
				end=(char*)strchr(begin, quote);
			}
			if(!end)
				end=(char*)strchr(begin, ';');

			if(end)
				*end=0; // terminator

			return *begin ? &pa_charsets.get_direct(begin) : 0;
		}
	}
	return 0;
}

const UTF16* pa_utf16_encode(const char* in, Charset& source_charset){
	if(!in)
		return 0;

	String::C sIn(in,strlen(in));

	UTF16* utf16=(UTF16*)pa_malloc_atomic(sIn.length*2+2);
	UTF16* utf16_end=utf16;

	if(!source_charset.isUTF8())
		sIn=Charset::transcode(sIn, source_charset, pa_UTF8_charset);

	int status=pa_convertUTF8toUTF16((const UTF8**)&sIn.str, (const UTF8*)(sIn.str+sIn.length), &utf16_end, utf16+sIn.length, strictConversion);
	if(status != conversionOK)
		throw Exception("utf-16 encode", new String(in), "utf-16 conversion failed (%d)", status);

	*utf16_end=0;

	return utf16;
}

const char* pa_utf16_decode(const UTF16* in, Charset& asked_charset){
	if(!in)
		return 0;

	const UTF16* utf16_start=in;
	const UTF16* utf16_end;

	for(utf16_end=in; *utf16_end; utf16_end++);

	char *result = (char *)pa_malloc_atomic((utf16_end-in)*6+1);
	char *result_end = result;

	int status=pa_convertUTF16toUTF8(&utf16_start, utf16_end, (UTF8**)&result_end, (UTF8*)(result+(utf16_end-in)*6), strictConversion);

	if(status != conversionOK)
		throw Exception("utf-16 decode", 0, "utf conversion failed (%d)", status);

	*result_end='\0';

	if(asked_charset.isUTF8())
		return result;

	return Charset::transcode(result, pa_UTF8_charset, asked_charset).cstr();
}

static bool is_latin(const char *in){
	for(; *in; in++){
		if ((unsigned char)(*in) > 0x7F)
			return false;
	}
	return true;
}

#define MAX_IDNA_LENGTH 256

const char *pa_idna_encode(const char *in, Charset& source_charset){
	if(!in || is_latin(in))
		return in;

	uint32_t utf32[MAX_IDNA_LENGTH];
	uint32_t *utf32_end=utf32;

	String::C sIn(in,strlen(in));

	if(!source_charset.isUTF8())
		sIn=Charset::transcode(sIn, source_charset, pa_UTF8_charset);

	int status=pa_convertUTF8toUTF32((const UTF8**)&sIn.str, (const UTF8*)(sIn.str+sIn.length), &utf32_end, utf32+MAX_IDNA_LENGTH-1, strictConversion);
	if(status != conversionOK)
		throw Exception("idna encode", new String(in), "utf conversion failed (%d)", status);

	*utf32_end=0;

	char *result = (char *)pa_malloc(MAX_IDNA_LENGTH);
	status=pa_idna_to_ascii_4z(utf32, result, MAX_IDNA_LENGTH, 0);
	if(status != IDNA_SUCCESS)
		throw Exception("idna encode", new String(in), "encode failed: %s", pa_idna_strerror(status));

	return result;
}

const char *pa_idna_decode(const char *in, Charset &asked_charset){
	if(!in || !(*in))
		return in;

	uint32_t utf32[MAX_IDNA_LENGTH];
	const uint32_t *utf32_start=utf32;
	uint32_t *utf32_end;

	int status=pa_idna_to_unicode_4z(in, utf32, MAX_IDNA_LENGTH, 0);
	if(status != IDNA_SUCCESS)
		throw Exception("idna decode", new String(in), "decode failed: %s", pa_idna_strerror(status));

	for(utf32_end=utf32; *utf32_end; utf32_end++);

	char *result = (char *)pa_malloc(MAX_IDNA_LENGTH);
	char *result_end = result;

	status=pa_convertUTF32toUTF8(&utf32_start, utf32_end, (UTF8**)&result_end, (UTF8*)(result+MAX_IDNA_LENGTH-1), strictConversion);

	if(status != conversionOK)
		throw Exception("idna decode", new String(in), "utf conversion failed (%d)", status);

	*result_end='\0';

	if(!asked_charset.isUTF8())
		result = (char *)Charset::transcode(result, pa_UTF8_charset, asked_charset).cstr();

	return result;
}
/// must be last in this file
#undef vsnprintf
int pa_vsnprintf(char* b, size_t s, const char* f, va_list l) {
	if(!s)
		return 0;

	int r;
	// note: on win32 & maybe somewhere else
	// vsnprintf do not writes terminating 0 in 'buffer full' case, reducing
	// http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
	--s;

	// clients do not check for negative 's', feature: ignore such prints
	if((ssize_t)s<0)
		return 0;

#ifdef _MSC_VER
	// win32: if the number of bytes to write exceeds buffer, then count bytes are written and -1 is returned
	r=_vsnprintf(b, s, f, l); 
	if(r<0) 
		r=s;
#else
	r=vsnprintf(b, s, f, l); 
	/*
	solaris: man vsnprintf

	The snprintf() function returns  the  number  of  characters
	formatted, that is, the number of characters that would have
	been written to the buffer if it were large enough.  If  the
	value  of  n  is  0  on a call to snprintf(), an unspecified
	value less than 1 is returned.
	*/

	if(r<0)
		r=0;
	else if((size_t)r>s)
		r=s;
#endif
	b[r]=0;
	return r;
}

int pa_snprintf(char* b, size_t s, const char* f, ...) {
	va_list l;
	va_start(l, f); 
	int r=pa_vsnprintf(b, s, f, l); 
	va_end(l); 
	return r;
}
