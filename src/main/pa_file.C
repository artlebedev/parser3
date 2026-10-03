/** @file
	Parser: file and file system related functions.

	Copyright (c) 2000-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_file.h"
#include "pa_dir.h"
#include "pa_charsets.h"
#include "pa_http.h"
#include "pa_convert_utf.h"

#ifdef _MSC_VER
#include <windows.h>
#include <direct.h>
#endif

volatile const char * IDENT_PA_FILE_C="$Id: pa_file.C,v 1.11 2026/10/03 14:30:03 moko Exp $" IDENT_PA_FILE_H;

// a tainted path scheme (file://, http:// or parser://) would be accepted
//#define IGNORE_FILE_PREFIX_LANGUAGES

// some maybe-undefined constants

#ifndef _O_TEXT
#	define _O_TEXT 0
#endif
#ifndef _O_BINARY
#	define _O_BINARY 0
#endif

#ifdef HAVE_FTRUNCATE
#	define PA_O_TRUNC 0
#else
#	ifdef _O_TRUNC
#		define PA_O_TRUNC _O_TRUNC
#	else
#		error you must have either ftruncate function or _O_TRUNC bit declared
#	endif
#endif

#ifdef WIN32

bool is_os_absolute_path(const String& path) {
	return is_os_absolute_path(path.cstr());
}

bool is_os_absolute_path(const char* path) {
	return path[0] && path[0]!=':' && path[1]==':' // DRIVE: (including drive-relative paths, for compatibility)
		|| path[0]=='\\' && path[1]=='\\';     // UNC, only '\\' is supported as '//' is a document root relative path
}
#endif

uint path_scheme(const String& path) {
	if(path.starts_with("file://"))
		return PA_SCHEME_FILE;
	if(path.starts_with("http://"))
		return PA_SCHEME_HTTP;
	if(path.starts_with("parser://"))
		return PA_SCHEME_PARSER;
	return PA_SCHEME_NONE;
}

bool clean_path_scheme(uint scheme, const String& path) {
#ifdef IGNORE_FILE_PREFIX_LANGUAGES
	return true;
#else
	return path.check_lang(String::L_AS_IS, 0, scheme==PA_SCHEME_PARSER ? 9 /* "parser://" */ : 7 /* "file://" or "http://" */);
#endif
}

void check_path_scheme(uint scheme, const String& path, uint allowed) {
	if(scheme!=PA_SCHEME_FILE && !(allowed&scheme))
		throw Exception(PARSER_RUNTIME, &path, "unsupported path scheme");
	if(!clean_path_scheme(scheme, path))
		throw Exception(PARSER_RUNTIME, &path, "tainted path scheme");
}

int path_cmp(const char* path, const char* dir, size_t length) {
#ifdef WIN32
	return strncasecmp(path, dir, length); // mostly for the drive letter
#else
	return strncmp(path, dir, length);
#endif
}

const char* actual_filename(const String& file_spec, const char* fname) {
	return strcmp(file_spec.cstr(), fname) ? pa_strcat(", actual filename '", fname, "'") : "";
}

const String* file_uri_to_path(const String& uri) {
	const String* rest=&uri.mid(7 /* "file://" */, uri.length());
	if(rest->starts_with("localhost/"))
		rest=&rest->mid(9 /* "localhost", keeping the slash */, rest->length());
	if(rest->first_char()=='/') {
		// file:///C:/path and file://localhost/C:/path: the slash before a drive name is not a part of the path
		const String& disk=rest->mid(1, rest->length());
		return is_os_absolute_path(disk) ? &disk : rest;
	}
	if(is_os_absolute_path(*rest))
		return rest; // file://C:/path, not a standard form but in use
#ifdef WIN32
	if(rest->pos('/')!=STRING_NOT_FOUND) { // file://server/share/path is the UNC path \\server\share\path
		String& result=*new String("//");
		result << *rest;
		return &result;
	}
#endif
	return 0;
}

// externs

const UTF16* pa_utf16_encode(const char* in, Charset& source_charset);

// functions

#ifdef _MSC_VER

#define PA_UTF16_ENC(value) (const wchar_t *)pa_utf16_encode(value, pa_thread_request().charsets.source())

int pa_stat(const char *pathname, struct stat *buffer){
	return _wstat64(PA_UTF16_ENC(pathname), buffer);
}

int pa_open(const char *pathname, int flags, int mode){
	return _wopen(PA_UTF16_ENC(pathname), flags, mode);
}

FILE *pa_fopen(const char *pathname, const char *mode){
	return _wfopen(PA_UTF16_ENC(pathname), PA_UTF16_ENC(mode));
}

int pa_mkdir(const char *pathname, int){
	return _wmkdir(PA_UTF16_ENC(pathname));
}

int pa_rmdir(const char *pathname){
	return _wrmdir(PA_UTF16_ENC(pathname));
}

int pa_rename(const char *oldpath, const char *newpath){
	return _wrename(PA_UTF16_ENC(oldpath), PA_UTF16_ENC(newpath));
}

int pa_unlink(const char *pathname){
	return _wunlink(PA_UTF16_ENC(pathname));
}

#else

#define pa_mkdir mkdir
#define pa_rmdir rmdir
#define pa_rename rename
#define pa_unlink unlink

#endif

/// these options were handled but not checked elsewhere, now check them
int pa_get_valid_file_options_count(HashStringValue& options) {
	int result=0;
	if(options.get(PA_SQL_LIMIT_NAME))
		result++;
	if(options.get(PA_SQL_OFFSET_NAME))
		result++;
	if(options.get(PA_COLUMN_SEPARATOR_NAME))
		result++;
	if(options.get(PA_COLUMN_ENCLOSER_NAME))
		result++;
	if(options.get(PA_CHARSET_NAME))
		result++;
	return result;
}

#ifndef DOXYGEN
struct File_read_action_info {
	char **data; size_t *data_size;
	char* buf; uint64_t offset; size_t limit;
};
#endif

static void file_read_action(struct stat& finfo, int f, const String& file_spec, void *context) {
	File_read_action_info& info = *static_cast<File_read_action_info *>(context);
	size_t to_read_size = check_file_size(info.limit && info.limit < (size_t)finfo.st_size ? info.limit : (size_t)finfo.st_size, &file_spec);
	if(to_read_size) {
		if(info.offset)
			 pa_lseek(f, info.offset, SEEK_SET); // seek never fails as POSIX allows the file offset to be set beyond the EOF
		*info.data = info.buf ? info.buf : (char *)pa_malloc_atomic(to_read_size+1);
		ssize_t result = read(f, *info.data, to_read_size);
		if(result<0)
			throw Exception("file.read", &file_spec, "read failed: %s (%d)", strerror(errno), errno);
		*info.data_size = result;
	} else { // empty file
		// for both, text and binary: for text we need that terminator, for binary we need nonzero pointer to be able to save such files
		*info.data = (char *)pa_malloc_atomic(1);
		*(char*)(*info.data) = 0;
		*info.data_size = 0;
		return;
	}
}

File_read_result file_read_binary(const String& file_spec, bool fail_on_read_problem, char* buf, uint64_t offset, size_t limit) {
	File_read_result result = {false, 0, 0, 0};
	File_read_action_info info = {&result.str, &result.length, buf, offset, limit};

	result.success = file_read_action_under_lock(file_spec, "read", file_read_action, &info, 0, fail_on_read_problem);
	return result;
}

File_read_result file_read(Request_charsets& charsets, const String& file_spec,
			bool as_text, HashStringValue *options,
			bool fail_on_read_problem,
			size_t offset = 0, size_t limit = 0, bool transcode_text_result = true) {
	File_read_result result = {false, 0, 0, 0};
	if(options){
		int valid_options = pa_get_valid_file_options_count(*options);
		if(valid_options != options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}

	File_read_action_info info = {&result.str, &result.length, 0, offset, limit};

	result.success = file_read_action_under_lock(file_spec, "read", file_read_action, &info, as_text, fail_on_read_problem);

	if(as_text){
		if(result.success){
			Charset* asked_charset = 0;
			if(options)
				if(Value* vcharset_name = options->get(PA_CHARSET_NAME))
					asked_charset = &pa_charsets.get(vcharset_name->as_string());

			asked_charset = pa_charsets.checkBOM(result.str, result.length, asked_charset);

			if(result.length && transcode_text_result && asked_charset){ // length must be checked because transcode returns CONST string in case length==0, which contradicts hacking few lines below
				String::C body = String::C(result.str, result.length);
				body=Charset::transcode(body, *asked_charset, charsets.source());

				result.str = const_cast<char*>(body.str); // hacking a little
				result.length = body.length;
			}
		}
		if(result.length)
			fix_line_breaks(result.str, result.length);
	}

	return result;
}

File_read_result file_load(Request& r, const String& file_spec,
			bool as_text, HashStringValue *options,
			bool fail_on_read_problem,
			bool transcode_text_result) {

	size_t offset = 0;
	size_t limit = 0;

	if(options){
		if(Value *voffset = (Value *)options->get(sql_offset_name))
			offset = r.process(*voffset).as_int();
		if(Value *vlimit = (Value *)options->get(sql_limit_name))
			limit = r.process(*vlimit).as_int();
		// no check on options count here
	}

	if(file_spec.starts_with("http://")) {
		if(offset || limit)
			throw Exception(PARSER_RUNTIME, 0, "offset and load options are not supported for HTTP:// file load");

		// fail on read problem
		File_read_http_result http = pa_internal_file_read_http(r, file_spec, as_text, options, transcode_text_result);

		File_read_result result = {true, http.str, http.length, http.headers};
		return result;
	} else
		return file_read(r.charsets, file_spec, as_text, options, fail_on_read_problem, offset, limit, transcode_text_result);
}

char* file_read_text(Request_charsets& charsets, const String& file_spec, bool fail_on_read_problem) {
	File_read_result file = file_read(charsets, file_spec, true, 0, fail_on_read_problem);
	return file.success ? file.str : 0;
}

char* file_load_text(Request& r, const String& file_spec, bool fail_on_read_problem, HashStringValue* options, bool transcode_result) {
	File_read_result file = file_load(r, file_spec, true, options, fail_on_read_problem, transcode_result);
	return file.success ? file.str : 0;
}

#ifdef PA_SAFE_MODE
void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname) {
	if(finfo.st_uid/*foreign?*/!=geteuid()
		&& finfo.st_gid/*foreign?*/!=getegid())
		throw Exception(PARSER_RUNTIME,
			&file_spec,
			"parser is in safe mode: reading files of foreign group and user disabled "
			"[recompile parser with --disable-safe-mode configure option], "
			"actual filename '%s', fuid(%d)!=euid(%d) or fgid(%d)!=egid(%d)",
			fname, finfo.st_uid, geteuid(), finfo.st_gid, getegid()
		);
}
#else
void check_safe_mode(struct stat, const String&, const char*) {
}
#endif


bool file_read_action_under_lock(const String& file_spec, const char* action_name, File_read_action action, void *context,
				bool as_text, bool fail_on_read_problem) {

	const char* fname=file_spec.taint_cstr(String::L_FILE_SPEC);
	int f;

	// first open, next stat:
	// directory update of NTFS hard links performed on open.
	// ex:
	//   a.html:^test[] and b.html hardlink to a.html
	//   user inserts ! before ^test in a.html
	//   directory entry of b.html in NTFS not updated at once,
	//   they delay update till open, so we would receive "!^test[" string
	//   if would do stat, next open.
	// later: it seems, even this does not help sometimes
	if((f=pa_open(fname, O_RDONLY | (as_text ? _O_TEXT : _O_BINARY) ))>=0) {
		try {
			int pa_errno=pa_lock_shared_blocking(f);
			if(pa_errno!=0)
				throw Exception("file.lock", &file_spec, "shared lock failed: %s (%d)%s", strerror(pa_errno), pa_errno, actual_filename(file_spec, fname));

			struct stat finfo;
			if(pa_fstat(f, &finfo)!=0)
				throw Exception("file.missing", // hardly possible: we just opened it OK
					&file_spec, "stat failed: %s (%d)%s", strerror(errno), errno, actual_filename(file_spec, fname));

			check_safe_mode(finfo, file_spec, fname);

			action(finfo, f, file_spec, context);
		} catch(...) {
			pa_unlock(f);close(f);
			if(fail_on_read_problem)
				rethrow;
			return false;
		}

		pa_unlock(f);close(f);
		return true;
	} else {
		if(fail_on_read_problem)
			throw Exception(errno==EACCES ? "file.access" : (errno==ENOENT || errno==ENOTDIR || errno==ENODEV) ? "file.missing" : 0,
				&file_spec, "%s failed: %s (%d)%s", action_name, strerror(errno), errno, actual_filename(file_spec, fname));
		return false;
	}
}

void create_dir_for_file(const String& file_spec) {
	const char *str=file_spec.taint_cstr(String::L_FILE_SPEC);
	if(str[0]){
		const char *pos=str+1;
		while((pos=strchr(pos, '/')) && pos[1]) { // to avoid trailing /, see #1166
			pa_mkdir(pa_strdup(str,pos-str), 0775);
			pos++;
		}
	}
}

bool file_write_action_under_lock(const String& file_spec, const char* action_name, File_write_action action, void *context,
				bool as_text, bool do_append, bool do_block, bool fail_on_lock_problem) {

	const char* fname=file_spec.taint_cstr(String::L_FILE_SPEC);
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec);

	if((f=pa_open(fname, O_CREAT | O_RDWR | (as_text ? _O_TEXT : _O_BINARY) | (do_append ? O_APPEND : PA_O_TRUNC), 0664))>=0) {
		int pa_errno=do_block ? pa_lock_exclusive_blocking(f) : pa_lock_exclusive_nonblocking(f);
		if(pa_errno!=0) {
			Exception e("file.lock", &file_spec, "exclusive lock failed: %s (%d)%s", strerror(pa_errno), pa_errno, actual_filename(file_spec, fname));
			close(f);
			if(fail_on_lock_problem)
				throw e;
			return false;
		}

		try {
#if (defined(HAVE_FCHMOD) && defined(PA_SAFE_MODE))
			struct stat finfo;
			if(pa_fstat(f, &finfo)==0 && finfo.st_mode & 0111)
				fchmod(f, finfo.st_mode & 0666/*clear executable bits*/); // backward: ignore errors if any
#endif
			action(f, context);
		} catch(...) {
#ifdef HAVE_FTRUNCATE
			if(!do_append)
				PA_UNUSED int ignore_result=ftruncate(f, lseek(f, 0, SEEK_CUR)); // one cannot use O_TRUNC, read lower
#endif
			pa_unlock(f);close(f);
			rethrow;
		}
		
#ifdef HAVE_FTRUNCATE
		if(!do_append)
			PA_UNUSED int ignore_result=ftruncate(f, lseek(f, 0, SEEK_CUR)); // O_TRUNC truncates even exclusevely write-locked file [thanks to Igor Milyakov <virtan@rotabanner.com> for discovering]
#endif
		pa_unlock(f);close(f);
		return true;
	} else
		throw Exception(errno==EACCES ? "file.access" : 0, &file_spec, "%s failed: %s (%d)%s", action_name, strerror(errno), errno, actual_filename(file_spec, fname));
	// here should be nothing, see rethrow above
}

#ifndef DOXYGEN
struct File_write_action_info {
	const char* str;
	size_t length;
};
#endif

// a single write() must stay below INT_MAX as windows _write() reports written bytes as int
#define FILE_WRITE_CHUNK_SIZE (256*0x100000)

static const char* bytes_left(size_t rest) {
	return rest ? pa_strcat(", ", pa_uitoa(rest), " bytes left") : "";
}

static void file_write_action(int f, void *context) {
	File_write_action_info& info=*static_cast<File_write_action_info *>(context);

	const char* data=info.str;
	size_t left=info.length;

	while(left) {
		size_t portion=min(left, (size_t)FILE_WRITE_CHUNK_SIZE);
		ssize_t written=write(f, data, portion);
		if(written<0)
			throw Exception("file.write", 0, "error writing %s bytes: %s (%d)%s", pa_uitoa(portion), strerror(errno), errno, bytes_left(left-portion));
		if(written==0)
			throw Exception("file.write", 0, "error writing %s bytes: zero bytes written%s", pa_uitoa(portion), bytes_left(left-portion));
		data+=written;
		left-=written;
	}
}

void file_write(
				Request_charsets& charsets,
				const String& file_spec,
				const char* data,
				size_t size,
				bool as_text,
				bool do_append,
				Charset* asked_charset) {

	if(as_text && asked_charset){
		String::C body=String::C(data, size);
		body=Charset::transcode(body, charsets.source(), *asked_charset);
		data=body.str;
		size=body.length;
	};

	File_write_action_info info={data, size};

	file_write_action_under_lock(
		file_spec,
		"write",
		file_write_action,
		&info,
		as_text,
		do_append);
}

static size_t get_dir(char* fname, size_t helper_length){
	bool dir=false;
	size_t pos=0;
	for(pos=helper_length; pos; pos--){
		char c=fname[pos-1];
		if(c=='/'){
			fname[pos-1]=0;
			dir=true;
		} else if(dir) break;
	}
	return pos;
}

bool entry_exists(const char* fname, struct stat *afinfo) {
	struct stat lfinfo;
	bool result=pa_stat(fname, &lfinfo)==0;
	if(afinfo)
		*afinfo=lfinfo;
	return result;
}

bool entry_exists(const String& file_spec) {
	return entry_exists(file_spec.taint_cstr(String::L_FILE_SPEC), 0);
}

static bool entry_ifdir(char *fname, bool need_dir) {
	if(need_dir){
		size_t size=strlen(fname);
		while(size) {
			char c=fname[size-1];
			if(c=='/')
				fname[--size]=0;
			else
				break;
		}
	}

	struct stat finfo;
	if(entry_exists(fname, &finfo)) {
		bool is_dir=(finfo.st_mode&S_IFDIR) != 0;
		return is_dir==need_dir;
	}
	return false;
}

static bool entry_ifdir(const String& file_spec, bool need_dir) {
	return entry_ifdir(file_spec.taint_cstrm(String::L_FILE_SPEC), need_dir);
}

// throws nothing! [this is required in file_move & file_delete]
static void rmdir(const String& file_spec, size_t pos_after) {
	char* dir_spec=file_spec.taint_cstrm(String::L_FILE_SPEC);
	size_t length=strlen(dir_spec);
	while( (length=get_dir(dir_spec, length)) && (length > pos_after) ){
#ifdef _MSC_VER
		if(!entry_ifdir(dir_spec, true))
			break;
		DWORD attrs=GetFileAttributesW(PA_UTF16_ENC(dir_spec));
		if(
			(attrs==INVALID_FILE_ATTRIBUTES)
			|| !(attrs & FILE_ATTRIBUTE_DIRECTORY)
			|| (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
		)
			break;
#endif
		if(pa_rmdir(dir_spec))
			break;
	};
}

bool file_delete(const String& file_spec, bool fail_on_problem, bool keep_empty_dirs) {
	const char* fname=file_spec.taint_cstr(String::L_FILE_SPEC);
	if(pa_unlink(fname)!=0) {
		if(fail_on_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0,
				&file_spec, "unlink failed: %s (%d)%s", strerror(errno), errno, actual_filename(file_spec, fname));
		else
			return false;
	}

	if(!keep_empty_dirs)
		rmdir(file_spec, 1);

	return true;
}

void file_move(const String& old_spec, const String& new_spec, bool keep_empty_dirs) {
	const char* old_spec_cstr=old_spec.taint_cstr(String::L_FILE_SPEC);
	const char* new_spec_cstr=new_spec.taint_cstr(String::L_FILE_SPEC);
	
	create_dir_for_file(new_spec);

	if(pa_rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(errno==EACCES ? "file.access" : errno==ENOENT ? "file.missing" : 0,
			&old_spec, "rename to '%s' failed: %s (%d)%s", new_spec_cstr, strerror(errno), errno, actual_filename(old_spec, old_spec_cstr));

	if(!keep_empty_dirs)
		rmdir(old_spec, 1);
}


bool file_exist(const String& file_spec) {
	return entry_ifdir(file_spec, false);
}

bool dir_exists(const String& file_spec) {
	return entry_ifdir(file_spec, true);
}

const String* file_exist(const String& path, const String& name) {
	String& result=*new String(path);
	if(path.last_char() != '/')
		result << "/";
	result << name;
	return file_exist(result)?&result:0;
}

bool file_executable(const String& file_spec) {
	return access(file_spec.taint_cstr(String::L_FILE_SPEC), X_OK)==0;
}

bool file_stat(const String& file_spec, uint64_t& rsize, time_t& ratime, time_t& rmtime, time_t& rctime, bool fail_on_read_problem) {
	const char* fname=file_spec.taint_cstr(String::L_FILE_SPEC);
	struct stat finfo;
	if(pa_stat(fname, &finfo)!=0) {
		if(fail_on_read_problem)
			throw Exception("file.missing", &file_spec, "getting file size failed: %s (%d)%s", strerror(errno), errno, actual_filename(file_spec, fname));
		else
			return false;
	}
	rsize=finfo.st_size;
	ratime=(time_t)finfo.st_atime;
	rmtime=(time_t)finfo.st_mtime;
	rctime=(time_t)finfo.st_ctime;
	return true;
}

size_t check_file_size(uint64_t size, const String* file_spec){
	if(size > (uint64_t)pa_file_size_limit)
		throw Exception(PARSER_RUNTIME, file_spec, "content size of %s bytes exceeds the limit (%s bytes)", pa_uitoa(size), pa_uitoa(pa_file_size_limit));
	return (size_t)size;
}

/**
	String related functions
*/

const char *pa_filename(const char *path){
	const char *slash=strrpbrk(path, "/\\");
	return slash ? slash+1 : path;
}
