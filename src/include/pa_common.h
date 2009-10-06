/** @file
	Parser: commonly used functions.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

static const char * const IDENT_COMMON_H="$Date: 2009/10/06 11:39:02 $";

#include "pa_string.h"
#include "pa_hash.h"

class Request;

// defines
#define HTTP_USER_AGENT	"user-agent"

#define HTTP_STATUS	"status"
#define HTTP_STATUS_CAPITALIZED	"Status"

#define HTTP_CONTENT_LENGTH	"content-length"
#define HTTP_CONTENT_LENGTH_CAPITALIZED	"Content-Length"

#define HTTP_CONTENT_TYPE	"content-type"
#define HTTP_CONTENT_TYPE_UPPER	"CONTENT-TYPE"
#define HTTP_CONTENT_TYPE_CAPITALIZED	"Content-Type"
#define HTTP_CONTENT_TYPE_FORM_URLENCODED	"application/x-www-form-urlencoded"
#define HTTP_CONTENT_TYPE_MULTIPART_FORMDATA	"multipart/form-data"
#define HTTP_CONTENT_TYPE_MULTIPART_RELATED	"multipart/related"
#define HTTP_CONTENT_TYPE_MULTIPART_MIXED	"multipart/mixed"

#define CONTENT_TRANSFER_ENCODING_NAME "content-transfer-encoding"
#define CONTENT_TRANSFER_ENCODING_CAPITALIZED "Content-Transfer-Encoding"

#define CONTENT_DISPOSITION	"content-disposition"
#define CONTENT_DISPOSITION_CAPITALIZED	"Content-Disposition"
#define CONTENT_DISPOSITION_ATTACHMENT "attachment"
#define CONTENT_DISPOSITION_INLINE "inline"
#define CONTENT_DISPOSITION_FILENAME_NAME "filename"

const String http_content_type(HTTP_CONTENT_TYPE);

const String content_transfer_encoding_name(CONTENT_TRANSFER_ENCODING_NAME);

const String content_disposition(CONTENT_DISPOSITION);
const String content_disposition_inline(CONTENT_DISPOSITION_INLINE);
const String content_disposition_attachment(CONTENT_DISPOSITION_ATTACHMENT);
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

#if _MSC_VER
/*
inline int open( const char* filename, int oflag ) { return _open(filename, oflag); }
inline int close( int handle ) { return _close(handle); }
inline int read( int handle, void *buffer, unsigned int count ) { return _read(handle,buffer,count); }
inline int write( int handle, const void *buffer, unsigned int count ) { return _write(handle,buffer,count); }
inline int stat( const char* path, struct _stat *buffer ) { return _stat(path, buffer); }
inline long lseek( int handle, long offset, int origin ) { return _lseek(handle, offset, origin); }
*/

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
#ifndef mkdir
#	define mkdir(path, mode) _mkdir(path)
#endif

#ifndef putenv
#	define putenv _putenv
#endif

#endif

const char* capitalize(const char* s);

/** under WIN32 "t" mode fixes DOS chars OK, 
	can't say that about other systems/ line break styles
*/
void fix_line_breaks(
			char *str,
			size_t& length///< may change! used to speedup next actions
			);

int pa_lock_shared_blocking(int fd);
int pa_lock_exclusive_blocking(int fd);
int pa_lock_exclusive_nonblocking(int fd);
int pa_unlock(int fd);

void create_dir_for_file(const String& file_spec);

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
bool file_delete(const String& file_spec, bool fail_on_problem=true);
/**
	move specified file 
	throws an exception in case of problems
*/
void file_move(const String& old_spec, const String& new_spec);

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

/**
	scans for @a delim[default \n] in @a *row_ref, 
	@return piece of line before it or end of string, if no @a delim found
	assigns @a *row_ref to point right after delimiter if there were one
	or to zero if no @a delim were found.
*/
char *getrow(char **row_ref,char delim='\n');
char *lsplit(char *string, char delim);
char *lsplit(char **string_ref,char delim);
char *rsplit(char *string, char delim);
const char* format(double value, char *fmt);

size_t stdout_write(const void *buf, size_t size);

char* unescape_chars(const char* cp, int len, Charset* client_charset=0, bool ignore_plus=false);

#ifdef WIN32
void back_slashes_to_slashes(char *s);
//void slashes_to_back_slashes(char *s);
#endif

#ifndef _qsort
#	define _qsort(names,cnt,sizeof_names,func_addr) \
		qsort(names,cnt,sizeof_names,func_addr)
#endif

bool StrStartFromNC(const char* str, const char* substr, bool equal=false);
size_t strpos(const char *str, const char *substr);

Charset* detect_charset(const char* content_type);

#define SECS_PER_DAY (60*60*24)
int getMonthDays(int year, int month);

int remove_crlf(char *start, char *end);

#ifdef PA_SAFE_MODE 
void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname); 
#endif 

void pa_base64_decode(const char *in, size_t in_size, char*& result, size_t& result_size);
char* pa_base64_encode(const char *in, size_t in_size);
struct File_base64_action_info {
	unsigned char** base64;
}; 
char* pa_base64_encode(const String& file_spec);
static void file_base64_file_action(
				struct stat& finfo, 
				int f, 
				const String&, const char* /*fname*/, bool, 
				void *context);

#define FILE_BUFFER_SIZE	4096
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

int file_block_read(const int f, unsigned char* buffer, const size_t size);

inline void CalcCrc32(const unsigned char byte, unsigned long &crc32)
{
	crc32 = ((crc32) >> 8) ^ crc32Table[(byte) ^ ((crc32) & 0x000000FF)];
}

const unsigned long pa_crc32(const char *in, size_t in_size);
const unsigned long pa_crc32(const String& file_spec);
static void file_crc32_file_action(
				struct stat& finfo, 
				int f, 
				const String&, const char* /*fname*/, bool, 
				void *context);

static const char* hex_string(unsigned char* bytes, size_t size, bool upcase) {
	char *bytes_hex=new(PointerFreeGC) char [size*2/*byte->hh*/+1/*for zero-teminator*/];
	unsigned char *src=bytes;
	unsigned char *end=bytes+size;
	char *dest=bytes_hex;

	const char *hex=upcase?"0123456789ABCDEF":"0123456789abcdef";

	for(; src<end; src++) {
		*dest++=hex[*src/0x10];
		*dest++=hex[*src%0x10];
	}
	*dest=0;

	return bytes_hex;
}

int pa_get_valid_file_options_count(HashStringValue& options);

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

static String::C date_gmt_string(tm* tms) {
	/// http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.3
	static const char month_names[12][4]={
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	static const char days[7][4]={
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	char *buf=new(PointerFreeGC) char[MAX_STRING];
	return String::C(buf, 
		snprintf(buf, MAX_STRING, "%s, %.2d %s %.4d %.2d:%.2d:%.2d GMT", 
		days[tms->tm_wday],
		tms->tm_mday,month_names[tms->tm_mon],tms->tm_year+1900,
		tms->tm_hour,tms->tm_min,tms->tm_sec));
}


// globals

extern const String file_status_name;

// global defines for file options which are handled but not checked elsewhere, we check them

#define PA_SQL_LIMIT_NAME "limit"
#define PA_SQL_OFFSET_NAME "offset"
#define PA_COLUMN_SEPARATOR_NAME "separator"
#define PA_COLUMN_ENCLOSER_NAME "encloser"
#define PA_CHARSET_NAME "charset"

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

