/** @file
	Parser: commonly functions.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_globals.h"
#include "pa_charsets.h"
#include "pa_http.h"
#include "pa_request_charsets.h"
#include "pcre.h"
#include "pa_request.h"

#include "pa_idna.h"
#include "pa_convert_utf.h"

#ifdef _MSC_VER
#include <windows.h>
#include <direct.h>
#endif

#ifdef _MSC_VER
#define pa_mkdir(path, mode) _mkdir(path)
#else
#define pa_mkdir(path, mode) mkdir(path, mode)
#endif

volatile const char * IDENT_PA_COMMON_C="$Id: pa_common.C,v 1.324 2020/12/30 18:06:42 moko Exp $" IDENT_PA_COMMON_H IDENT_PA_HASH_H IDENT_PA_ARRAY_H IDENT_PA_STACK_H; 

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

// forwards

const UTF16* pa_utf16_encode(const char* in, Charset& source_charset);

// functions

#ifdef _MSC_VER

int pa_stat(const char *pathname, struct stat *buffer){
	const UTF16* utf16name=pa_utf16_encode(pathname, pa_thread_request().charsets.source());
	return _wstat64((const wchar_t *)utf16name, buffer);
}

int pa_open(const char *pathname, int flags, int mode){
	const UTF16* utf16name=pa_utf16_encode(pathname, pa_thread_request().charsets.source());
	return _wopen((const wchar_t *)utf16name, flags, mode);
}

FILE *pa_fopen(const char *pathname, const char *mode){
	const UTF16* utf16name=pa_utf16_encode(pathname, pa_thread_request().charsets.source());
	const UTF16* utf16mode=pa_utf16_encode(mode, pa_thread_request().charsets.source());
	return _wfopen((const wchar_t *)utf16name, (const wchar_t *)utf16mode);
}

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
	size_t to_read_size = check_file_size(info.limit && info.limit < finfo.st_size ? info.limit : finfo.st_size, &file_spec);
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



bool file_read_action_under_lock(const String& file_spec, 
				const char* action_name, File_read_action action, void *context, 
				bool as_text, 
				bool fail_on_read_problem) {
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
	if((f=pa_open(fname, O_RDONLY|(as_text?_O_TEXT:_O_BINARY)))>=0) {
		try {
			if(pa_lock_shared_blocking(f)!=0)
				throw Exception("file.lock", &file_spec, "shared lock failed: %s (%d), actual filename '%s'", strerror(errno), errno, fname);

			struct stat finfo;
			if(pa_fstat(f, &finfo)!=0)
				throw Exception("file.missing", // hardly possible: we just opened it OK
					&file_spec, "stat failed: %s (%d), actual filename '%s'", strerror(errno), errno, fname);

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
				&file_spec, "%s failed: %s (%d), actual filename '%s'", action_name, strerror(errno), errno, fname);
		return false;
	}
}

void create_dir_for_file(const String& file_spec) {
	const char *str=file_spec.taint_cstr(String::L_FILE_SPEC);
	if(str[0]){
		const char *pos=str+1;
		while((pos=strchr(pos,'/')) && pos[1]) { // to avoid trailing /, see #1166
			pa_mkdir(pa_strdup(str,pos-str), 0775);
			pos++;
		}
	}
}

bool file_write_action_under_lock(
				const String& file_spec, 
				const char* action_name,
				File_write_action action,
				void *context, 
				bool as_text, 
				bool do_append, 
				bool do_block, 
				bool fail_on_lock_problem) {
	const char* fname=file_spec.taint_cstr(String::L_FILE_SPEC); 
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec); 

	if((f=pa_open(fname, 
		O_CREAT|O_RDWR
		|(as_text?_O_TEXT:_O_BINARY)
		|(do_append?O_APPEND:PA_O_TRUNC), 0664))>=0) {
		if((do_block?pa_lock_exclusive_blocking(f):pa_lock_exclusive_nonblocking(f))!=0) {
			Exception e("file.lock", &file_spec, "shared lock failed: %s (%d), actual filename '%s'", strerror(errno), errno, fname);
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
				ftruncate(f, lseek(f, 0, SEEK_CUR)); // one can not use O_TRUNC, read lower
#endif
			pa_unlock(f);close(f); 
			rethrow;
		}
		
#ifdef HAVE_FTRUNCATE
		if(!do_append)
			ftruncate(f, lseek(f, 0, SEEK_CUR)); // O_TRUNC truncates even exclusevely write-locked file [thanks to Igor Milyakov <virtan@rotabanner.com> for discovering]
#endif
		pa_unlock(f);close(f); 
		return true;
	} else
		throw Exception(errno==EACCES ? "file.access" : 0, &file_spec, "%s failed: %s (%d), actual filename '%s'", action_name, strerror(errno), errno, fname);
	// here should be nothing, see rethrow above
}

#ifndef DOXYGEN
struct File_write_action_info {
	const char* str;
	size_t length;
}; 
#endif

static void file_write_action(int f, void *context) {
	File_write_action_info& info=*static_cast<File_write_action_info *>(context); 
	if(info.length) {
		ssize_t written=write(f, info.str, info.length); 
		if(written<0)
			throw Exception("file.write", 0, "write failed: %s (%d)",  strerror(errno), errno); 
		if((size_t)written!=info.length)
			throw Exception("file.write", 0, "write failed: %u of %u bytes written", written, info.length);
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
		if(c=='/' || c=='\\'){
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
			if(c=='/' || c=='\\')
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
		DWORD attrs=GetFileAttributes(dir_spec);
		if(
			(attrs==INVALID_FILE_ATTRIBUTES)
			|| !(attrs & FILE_ATTRIBUTE_DIRECTORY)
			|| (attrs & FILE_ATTRIBUTE_REPARSE_POINT)
		)
			break;
#endif
		if( rmdir(dir_spec) )
			break;
	};
}

bool file_delete(const String& file_spec, bool fail_on_problem, bool keep_empty_dirs) {
	const char* fname=file_spec.taint_cstr(String::L_FILE_SPEC); 
	if(unlink(fname)!=0) {
		if(fail_on_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0,
				&file_spec, "unlink failed: %s (%d), actual filename '%s'", strerror(errno), errno, fname);
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

	if(rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(errno==EACCES ? "file.access" : errno==ENOENT ? "file.missing" : 0,
			&old_spec, "rename failed: %s (%d), actual filename '%s' to '%s'", strerror(errno), errno, old_spec_cstr, new_spec_cstr);

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
			throw Exception("file.missing", &file_spec, "getting file size failed: %s (%d), real filename '%s'", strerror(errno), errno, fname);
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
		throw Exception(PARSER_RUNTIME, file_spec, "content size of %.15g bytes exceeds the limit (%.15g bytes)", (double)size, (double)pa_file_size_limit);
	return (size_t)size;
}

/** 
	String related functions
*/

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

const char *pa_filename(const char *path){
	if(!path)
		return NULL;
	for(const char *c = path + strlen(path) - 1; c >= path; c--) {
		if(*c == '/' || *c == '\\')
			return c+1;
        }
        return path;
}

// format: %[flags][width][.precision]type	http://msdn.microsoft.com/ru-ru/library/56e442dc(en-us,VS.80).aspx
//		flags: '-', '+', ' ', '#', '0'		http://msdn.microsoft.com/ru-ru/library/8aky45ct(en-us,VS.80).aspx
//		width, precision: non negative decimal number
enum FormatType {
	FormatInvalid,
	FormatInt,
	FormatUInt,
	FormatDouble
};
FormatType format_type(const char* fmt){
	enum FormatState {
		Percent, 
		Flags, 
		Width,
		Precision,
		Done
	} state=Percent;

	FormatType result=FormatInvalid;

	const char* pos=fmt;
	while(char c=*(pos++)){
		switch(state){
			case Percent:
				if(c=='%'){
					state=Flags;
				} else {
					return FormatInvalid; // 1st char must be '%' only
				}
				break;
			case Flags:
				if(strchr("-+ #0", c)!=0){
					break;
				}
				// go to the next step
			case Width:
				if(c=='.'){
					state=Precision;
					break;
				}
				// go to the next step
			case Precision:
				if(c>='0' && c<='9'){
					if(state == Flags) state=Width; // no more flags
					break;
				} else if(c=='d' || c=='i'){
					result=FormatInt;
				} else if(strchr("feEgG", c)!=0){
					result=FormatDouble;
				} else if(strchr("uoxX", c)!=0){
					result=FormatUInt;
				} else {
					return FormatInvalid; // invalid char
				}
				state=Done;
				break;
			case Done:
				return FormatInvalid; // no chars allowed after 'type'
		}
	}
	return result;
}


const char* format(double value, const char* fmt) {
	char local_buf[MAX_NUMBER];
	int size=-1;

	if(fmt && strlen(fmt)){
		switch(format_type(fmt)){
			case FormatDouble:
				size=snprintf(local_buf, sizeof(local_buf), fmt, value); 
				break;
			case FormatInt:
				size=snprintf(local_buf, sizeof(local_buf), fmt, (int)value); 
				break;
			case FormatUInt:
				size=snprintf(local_buf, sizeof(local_buf), fmt, (uint)value); 
				break;
			case FormatInvalid:
				throw Exception(PARSER_RUNTIME, 0, "Incorrect format string '%s' was specified.", fmt);
		}
	} else
		size=snprintf(local_buf, sizeof(local_buf), "%d", (int)value);

	if(size < 0 || size >= MAX_NUMBER-1){ // on win32 we manually reduce max size while printing
		throw Exception(PARSER_RUNTIME, 0, "Error occurred white executing snprintf with format string '%s'.", fmt);
	}

	return pa_strdup(local_buf, (size_t)size);
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
	char* dst=s;
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
							charset->store_Char((XMLByte*&)dst, (XMLCh)escapedValue, '?');
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

#ifdef WIN32
void back_slashes_to_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/'; 
}
#endif

size_t strpos(const char *str, const char *substr) {
	const char *p = strstr(str, substr);
	return (p==0)?STRING_NOT_FOUND:p-str;
}

int remove_crlf(char* start, char* end) {
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

