/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#define IDENT_PA_COMMON_H "$Id: pa_common.h,v 1.201 2026/09/30 16:44:19 moko Exp $"

#include "pa_string.h"
#include "pa_hash.h"

class Request;

// defines
#define HTTP_STATUS		"status"
#define HTTP_STATUS_CAPITALIZED	"Status"

#define HTTP_CONTENT_LENGTH		"content-length"

#define HTTP_CONTENT_TYPE		"content-type"
#define HTTP_CONTENT_TYPE_UPPER		"CONTENT-TYPE"
#define HTTP_CONTENT_TYPE_CAPITALIZED	"Content-Type"

#define CONTENT_DISPOSITION		"content-disposition"
#define CONTENT_DISPOSITION_UPPER	"CONTENT-DISPOSITION"
#define CONTENT_DISPOSITION_CAPITALIZED	"Content-Disposition"

#define CONTENT_DISPOSITION_ATTACHMENT	"attachment"
#define CONTENT_DISPOSITION_INLINE	"inline"
#define CONTENT_DISPOSITION_FILENAME_NAME "filename"

#define HTTP_CONTENT_TYPE_FORM_URLENCODED	"application/x-www-form-urlencoded"
#define HTTP_CONTENT_TYPE_MULTIPART_FORMDATA	"multipart/form-data"
#define HTTP_CONTENT_TYPE_MULTIPART_RELATED	"multipart/related"
#define HTTP_CONTENT_TYPE_MULTIPART_MIXED	"multipart/mixed"

extern const String content_disposition_filename_name;

#define HASH_ORDER

#ifdef HASH_ORDER
#undef PA_HASH_CLASS
#include "pa_hash.h"
#endif

class Value;
typedef HASH_STRING<Value*> HashStringValue;

// replace system s*nprintf with our versions
#undef vsnprintf
int pa_vsnprintf(char *, size_t, const char* , va_list);
#define vsnprintf pa_vsnprintf 
#undef snprintf
int pa_snprintf(char *, size_t, const char* , ...);
#define snprintf pa_snprintf

#ifdef _MSC_VER

#ifndef strcasecmp
#	define strcasecmp _stricmp
#endif

#ifndef strncasecmp
#	define strncasecmp _strnicmp
#endif

#endif

/**
	String related functions
*/

// Under WIN32 "t" mode fixes DOS chars OK, can't say that about other systems/ line break styles
void fix_line_breaks(char *str,	size_t& length /* < may change! used to speedup next actions */);

char *getrow(char **row_ref,char delim='\n');

// Return the last occurrence of any character in chars or NULL if absent
const char *strrpbrk(const char *string, const char *chars);
inline char *strrpbrk(char *string, const char *chars) {
	return (char*)strrpbrk((const char*)string, chars);
}

// Split at the first occurrence of delimiter; return the suffix or NULL if absent
char *lsplit(char *string, char delim);
// Return the prefix; advance *string_ref to the suffix or set it to NULL if delimiter is absent
char *lsplit(char **string_ref, char delim);

// Split at the last occurrence of delimiter(s); return the suffix or NULL if absent
char *rsplit(char *string, char delim);
char *rsplit(char *string, const char *delims);

char* unescape_chars(const char* cp, int len, Charset* client_charset=0, bool js=false/*true==decode \uXXXX and don't convert '+' to space*/);

char *search_stop(char*& current, char cstop_at);

inline int pa_strncasecmp(const char* str, const char* substr, size_t count=0) {
	return strncasecmp(str, substr, count ? count : strlen(substr));
}

// copy of the path with '/' only
char* backslashes_to_slashes(const char* path);
// same, keeping the string languages
const String& backslashes_to_slashes(const String& path);

size_t strpos(const char *str, const char *substr);

size_t remove_crlf(char *start, char *end);

inline bool pa_isalpha(unsigned char c) { return (((c>='A') && (c<='Z')) || ((c>='a') && (c<='z'))); }
inline bool pa_isalnum(unsigned char c) { return (((c>='0') && (c<='9')) || pa_isalpha(c)); }

void pa_strncpy(char *dst, const char *src, size_t dst_size);
char *pa_strcat(const char *a, const char *b, const char *c = 0);

const char* capitalize(const char* s);
char *str_lower(const char *s, size_t length);
char *str_upper(const char *s, size_t length);
inline char *str_lower(const char *s) { return str_lower(s, strlen(s)); }
inline char *str_upper(const char *s) { return str_upper(s, strlen(s)); }

const char* hex_string(unsigned char* bytes, size_t size, bool upcase);
extern const char* hex_digits;

const char *pa_idna_encode(const char *in, Charset &source);
const char *pa_idna_decode(const char *in, Charset &source);

unsigned long pa_crc32(const char *in, size_t in_size);
unsigned long pa_crc32(const String& file_spec);

/**
	Mix functions
*/

#define PA_DEFAULT(A,B) ((A) ? (A):(B) )

Charset* detect_charset(const char* content_type);

// globals

extern const String file_status_name;

// global defines for file options which are handled but not checked elsewhere, we check them

#define PA_COLUMN_SEPARATOR_NAME "separator"
#define PA_COLUMN_ENCLOSER_NAME "encloser"
#define PA_CHARSET_NAME "charset"
#define PA_RESPONSE_CHARSET_NAME "response-charset"

// globals defines for sql options

#define SQL_BIND_NAME "bind"
#define PA_SQL_LIMIT_NAME "limit"
#define PA_SQL_OFFSET_NAME "offset"
#define SQL_DEFAULT_NAME "default"
#define SQL_DISTINCT_NAME "distinct"
#define SQL_VALUE_TYPE_NAME "type"

extern String sql_bind_name;
extern String sql_limit_name;
extern String sql_offset_name;
extern String sql_default_name;
extern String sql_distinct_name;
extern String sql_value_type_name;

#ifndef DOXYGEN
enum Table2hash_distint { D_ILLEGAL, D_FIRST };
enum Table2hash_value_type { C_HASH, C_STRING, C_TABLE, C_CODE };
#endif

#endif
