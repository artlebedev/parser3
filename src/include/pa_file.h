/** @file
	Parser: file and file system related functions.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_FILE_H
#define PA_FILE_H

#define IDENT_PA_FILE_H "$Id: pa_file.h,v 1.7 2026/10/03 14:30:03 moko Exp $"

#include "pa_common.h"

#ifdef _MSC_VER

//access
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#define stat __stat64
#define pa_fstat _fstat64

int pa_stat(const char *pathname, struct stat *buffer);
int pa_open(const char *pathname, int flags, int mode=0);
FILE *pa_fopen(const char *pathname, const char *mode);

#define pa_lseek _lseeki64

#else

#define pa_stat stat
#define pa_fstat fstat

#define pa_open open
#define pa_fopen fopen

#define pa_lseek lseek

#endif

/// the path should not be '/'-normalized yet, as \\server would become the web path //server
#ifdef WIN32
bool is_os_absolute_path(const String& path);
bool is_os_absolute_path(const char* path);
#else
inline bool is_os_absolute_path(const String&) { return false; }
inline bool is_os_absolute_path(const char*) { return false; }
#endif

/// recognized path scheme, also used as a bit mask of the schemes a caller allows
enum { PA_SCHEME_NONE=0, PA_SCHEME_FILE=1, PA_SCHEME_HTTP=2, PA_SCHEME_PARSER=4 };
enum { PA_ALLOW_HTTP=PA_SCHEME_HTTP, PA_ALLOW_ALL=PA_SCHEME_HTTP|PA_SCHEME_PARSER };

/// scheme of a not '/'-normalized path: file://, http:// or parser://, PA_SCHEME_NONE otherwise
uint path_scheme(const String& path);

/// false if the scheme is tainted
bool clean_path_scheme(uint scheme, const String& path);

/// throws unless the scheme is allowed, file:// always is, or if the scheme is tainted
void check_path_scheme(uint scheme, const String& path, uint allowed);

/// resolved file:// or null; the result is not '/'-normalized
const String* file_uri_to_path(const String& uri);

/// strncmp for disk paths, case insensitive under windows
int path_cmp(const char* path, const char* dir, size_t length);

/// returns ", actual filename '...'" only if escaping changed the file name (exception source shows the original file)
const char* actual_filename(const String& file_spec, const char* fname);

#define FILE_BUFFER_SIZE (128*0x400)

int pa_lock_shared_blocking(int fd);
int pa_lock_exclusive_blocking(int fd);
int pa_lock_exclusive_nonblocking(int fd);
int pa_unlock(int fd);

const char *pa_filename(const char *path);

void create_dir_for_file(const String& file_spec);

int pa_get_valid_file_options_count(HashStringValue& options);

typedef void (*File_read_action)(struct stat& finfo, int f, const String& file_spec, void *context);

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
	read specified text file
	if fail_on_read_problem is true[default] throws an exception

	WARNING: charset is used for http header case conversion, it's not a charset of input file!
*/
char *file_read_text(Request_charsets& charsets, const String& file_spec, bool fail_on_read_problem = true);
char *file_load_text(Request& r, const String& file_spec, bool fail_on_read_problem = true, HashStringValue* options = 0, bool transcode_result = true);

struct File_read_result {
	bool success;
	char* str; size_t length;
	HashStringValue* headers;
};

// read specified binary file; if fail_on_read_problem is true[default] throws an exception
File_read_result file_read_binary(const String& file_spec, bool fail_on_read_problem = true, char* buf = 0, uint64_t offset = 0, size_t limit = 0);

// load specified file; if fail_on_read_problem is true[default] throws an exception
File_read_result file_load(Request& r, const String& file_spec, bool as_text, HashStringValue* options=0, bool fail_on_read_problem=true, bool transcode_text_result=true);

typedef void (*File_write_action)(int f, void *context);

/**
	lock specified file exclusively, do actions under lock.
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

// write data to specified file; throws an exception in case of problems
void file_write(
				Request_charsets& charsets,
				const String& file_spec,
				const char* data,
				size_t size,
				bool as_text,
				bool do_append=false,
				Charset* asked_charset=0);

// delete specified file; throws an exception in case of problems
bool file_delete(const String& file_spec, bool fail_on_problem=true, bool keep_empty_dirs=false);

// move specified file; throw an exception in case of problems
void file_move(const String& old_spec, const String& new_spec, bool keep_empty_dirs=false);

bool entry_exists(const char* fname, struct stat *afinfo=0);
bool entry_exists(const String& file_spec);
bool file_exist(const String& file_spec);
bool dir_exists(const String& file_spec);
const String* file_exist(const String& path, const String& name);
bool file_executable(const String& file_spec);

bool file_stat(const String& file_spec, uint64_t& rsize, time_t& ratime, time_t& rmtime, time_t& rctime, bool fail_on_read_problem=true);
size_t check_file_size(uint64_t size, const String* file_spec);

size_t stdout_write(const void *buf, size_t size);

void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname); 

ssize_t file_block_read(const int f, void* buffer, const size_t size);

#endif
