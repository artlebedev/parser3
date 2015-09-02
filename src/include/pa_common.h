/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#define IDENT_PA_COMMON_H "$Id: pa_common.h,v 1.160 2015/09/02 21:29:44 moko Exp $"

#include "pa_string.h"
#include "pa_hash.h"

class Request;

// defines
#define HTTP_USER_AGENT	"user-agent"

#define HTTP_STATUS	"status"
#define HTTP_STATUS_CAPITALIZED	"Status"

#define HTTP_CONTENT_LENGTH	"content-length"
#define HTTP_CONTENT_LENGTH_CAPITALIZED	"Content-Length"

#define HTTP_CONTENT_TYPE		"content-type"
#define HTTP_CONTENT_TYPE_UPPER		"CONTENT-TYPE"
#define HTTP_CONTENT_TYPE_CAPITALIZED	"Content-Type"
#define HTTP_CONTENT_TYPE_FORM_URLENCODED	"application/x-www-form-urlencoded"
#define HTTP_CONTENT_TYPE_MULTIPART_FORMDATA	"multipart/form-data"
#define HTTP_CONTENT_TYPE_MULTIPART_RELATED	"multipart/related"
#define HTTP_CONTENT_TYPE_MULTIPART_MIXED	"multipart/mixed"

#define CONTENT_DISPOSITION		"content-disposition"
#define CONTENT_DISPOSITION_UPPER	"CONTENT-DISPOSITION"
#define CONTENT_DISPOSITION_CAPITALIZED	"Content-Disposition"
#define CONTENT_DISPOSITION_ATTACHMENT "attachment"
#define CONTENT_DISPOSITION_INLINE "inline"
#define CONTENT_DISPOSITION_FILENAME_NAME "filename"

#define BASE64_STRICT_OPTION_NAME "strict"

const String content_disposition_filename_name(CONTENT_DISPOSITION_FILENAME_NAME);

#define HASH_ORDER

#ifdef HASH_ORDER
#undef PA_HASH_CLASS
#include "pa_hash.h"
#endif

class Value;
typedef HASH_STRING<Value*> HashStringValue;

// replace system s*nprintf with our versions
#undef vsnprintf 
int __vsnprintf(char *, size_t, const char* , va_list);
#define vsnprintf __vsnprintf 
#undef snprintf
int __snprintf(char *, size_t, const char* , ...);
#define snprintf __snprintf

#ifdef _MSC_VER

//access
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#ifndef strcasecmp
#	define strcasecmp _stricmp
#endif

#ifndef strncasecmp
#	define strncasecmp _strnicmp
#endif

#endif

/** 
	file related functions
*/

#define FILE_BUFFER_SIZE	4096

int pa_lock_shared_blocking(int fd);
int pa_lock_exclusive_blocking(int fd);
int pa_lock_exclusive_nonblocking(int fd);
int pa_unlock(int fd);

void create_dir_for_file(const String& file_spec);

int pa_get_valid_file_options_count(HashStringValue& options);

typedef void (*File_read_action)(
				struct stat& finfo,
				int f, 
				const String& file_spec, const char* fname, bool as_text,
				void *context);

/**
	shared-lock specified file, 
	do actions under lock.
	if fail_on_read_problem is true[default] throws an exception
	
	@returns true if read OK
*/
bool file_read_action_under_lock(const String& file_spec, 
				const char* action_name, File_read_action action, void *context,
				bool as_text=false,
				bool fail_on_read_problem=true);

/**
	read specified text file using 
	if fail_on_read_problem is true[default] throws an exception

	WARNING: charset is used for http header case conversion, it's not a charset of input file!
*/
char *file_read_text(Request_charsets& charsets, 
					const String& file_spec, 
					bool fail_on_read_problem=true,
					HashStringValue* options=0,
					bool transcode_result=true);

char *file_load_text(Request& r, 
					const String& file_spec, 
					bool fail_on_read_problem=true,
					HashStringValue* options=0,
					bool transcode_result=true);

struct File_read_result {
	bool success;
	char* str; size_t length;
	HashStringValue* headers;
};

/**
	read specified file using 
	if fail_on_read_problem is true[default] throws an exception

	WARNING: charset is used for http header case conversion, it's not a charset of input file!
*/
File_read_result file_read(Request_charsets& charsets,
				const String& file_spec,
				bool as_text,
				HashStringValue* options=0,
				bool fail_on_read_problem=true,
				char* buf=0, size_t offset=0, size_t size=0, bool transcode_text_result=true);

File_read_result file_load(Request& r,
				const String& file_spec,
				bool as_text,
				HashStringValue* options=0,
				bool fail_on_read_problem=true,
				char* buf=0, size_t offset=0, size_t size=0, bool transcode_text_result=true);

typedef void (*File_write_action)(int f, void *context);

/**
	lock specified file exclusively, 
	do actions under lock.
	throws an exception in case of problems
	
	if block=false does non-blocking lock
	@returns true if locked OK, or false if non-blocking locking failed
*/
bool file_write_action_under_lock(
				const String& file_spec, 
				const char* action_name,
				File_write_action action,
				void *context,
				bool as_text=false,
				bool do_append=false,
				bool do_block=true,
				bool fail_on_lock_problem=true);

/**
	write data to specified file, 
	throws an exception in case of problems
*/
void file_write(
				Request_charsets& charsets,
				const String& file_spec,
				const char* data,
				size_t size, 
				bool as_text,
				bool do_append=false,
				Charset* asked_charset=0);

/**
	delete specified file 
	throws an exception in case of problems
*/
bool file_delete(const String& file_spec, bool fail_on_problem=true, bool keep_empty_dirs=false);
/**
	move specified file 
	throws an exception in case of problems
*/
void file_move(const String& old_spec, const String& new_spec, bool keep_empty_dirs=false);

bool entry_exists(const char* fname, struct stat *afinfo=0);
bool entry_exists(const String& file_spec);
bool file_exist(const String& file_spec);
bool dir_exists(const String& file_spec);
const String* file_exist(const String& path, const String& name);
bool file_executable(const String& file_spec);

bool file_stat(const String& file_spec, 
				size_t& rsize, 
				time_t& ratime,
				time_t& rmtime,
				time_t& rctime,
				bool fail_on_read_problem=true);

size_t stdout_write(const void *buf, size_t size);

void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname); 

int file_block_read(const int f, unsigned char* buffer, const size_t size);

/** 
	String related functions
*/

const char* capitalize(const char* s);

/** under WIN32 "t" mode fixes DOS chars OK, 
	can't say that about other systems/ line break styles
*/
void fix_line_breaks(char *str,	size_t& length /* < may change! used to speedup next actions */);

char *getrow(char **row_ref,char delim='\n');
char *lsplit(char *string, char delim);
char *lsplit(char **string_ref,char delim);
char *rsplit(char *string, char delim);
const char* format(double value, const char *fmt);

char* unescape_chars(const char* cp, int len, Charset* client_charset=0, bool js=false/*true==decode \uXXXX and don't convert '+' to space*/);

char *search_stop(char*& current, char cstop_at);

inline int pa_strncasecmp(const char* str, const char* substr, size_t count=0) {
	return strncasecmp(str, substr, count ? count : strlen(substr));
}

#ifdef WIN32
void back_slashes_to_slashes(char *s);
#endif

size_t strpos(const char *str, const char *substr);

int remove_crlf(char *start, char *end);

inline bool pa_isalpha(unsigned char c) {
	return (((c>='A') && (c<='Z')) || ((c>='a') && (c<='z')));
}

inline bool pa_isalnum(unsigned char c) {
	return (((c>='0') && (c<='9')) || pa_isalpha(c));
}

const char* hex_string(unsigned char* bytes, size_t size, bool upcase);
extern const char* hex_digits;

void pa_base64_decode(const char *in, size_t in_size, char*& result, size_t& result_size, bool strict=false);
char* pa_base64_encode(const char *in, size_t in_size);
char* pa_base64_encode(const String& file_spec);

const char *pa_idna_encode(const char *in, Charset &source);
const char *pa_idna_decode(const char *in, Charset &source);

const unsigned long pa_crc32(const char *in, size_t in_size);
const unsigned long pa_crc32(const String& file_spec);

/** 
	Mix functions
*/

// some stuff for use with .for_each
static void copy_all_overwrite_to(
								HashStringValue::key_type key, 
								HashStringValue::value_type value, 
								HashStringValue* dest) {
	dest->put(key, value);
}

static void remove_key_from(
							HashStringValue::key_type key, 
							HashStringValue::value_type /*value*/, 
							HashStringValue* dest) {
	dest->remove(key);
}

Charset* detect_charset(const char* content_type);

// globals

extern const String file_status_name;

// global defines for file options which are handled but not checked elsewhere, we check them

#define PA_SQL_LIMIT_NAME "limit"
#define PA_SQL_OFFSET_NAME "offset"
#define PA_COLUMN_SEPARATOR_NAME "separator"
#define PA_COLUMN_ENCLOSER_NAME "encloser"
#define PA_CHARSET_NAME "charset"
#define PA_RESPONSE_CHARSET_NAME "response-charset"

// globals defines for sql options

#define SQL_BIND_NAME "bind"
#define SQL_DEFAULT_NAME "default"
#define SQL_DISTINCT_NAME "distinct"
#define SQL_VALUE_TYPE_NAME "type"

#ifndef DOXYGEN
enum Table2hash_distint { D_ILLEGAL, D_FIRST };
enum Table2hash_value_type { C_HASH, C_STRING, C_TABLE };
#endif

#endif

