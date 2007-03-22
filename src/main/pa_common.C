/** @file
	Parser: commonly functions.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

 * BASE64 part
 *  Authors: Michael Zucchi <notzed@ximian.com>
 *           Jeffrey Stedfast <fejj@ximian.com>
 *
 *  Copyright 2000-2004 Ximian, Inc. (www.ximian.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 *
 */

static const char * const IDENT_COMMON_C="$Date: 2007/03/22 18:58:51 $"; 

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_hash.h"
#include "pa_globals.h"
#include "pa_charsets.h"
#include "pa_http.h"
#include "pa_request_charsets.h"

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

// functions

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
			memcpy(dest, bol, len); 
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
		memcpy(dest, bol, eob-bol); 
	str[length]=0; // terminating
}

char* file_read_text(Request_charsets& charsets, 
		     const String& file_spec, 
		     bool fail_on_read_problem,
		     HashStringValue* params/*, HashStringValue* * out_fields*/) {
	File_read_result file=
		file_read(charsets, file_spec, true, params, fail_on_read_problem);
	return file.success?file.str:0;
}

/// these options were handled but not checked elsewhere, now check them
int pa_get_valid_file_options_count(HashStringValue& options)
{
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
	char* buf; size_t offset; size_t count;
}; 
#endif
static void file_read_action(
			     struct stat& finfo, 
			     int f, 
			     const String& file_spec, const char* /*fname*/, bool as_text, 
			     void *context) {
	File_read_action_info& info=*static_cast<File_read_action_info *>(context); 
	size_t to_read_size=info.count;
	if(!to_read_size)
		to_read_size=(size_t)finfo.st_size;
	assert( !(info.buf && as_text) );
	if(to_read_size) { 
		if(info.offset)
			lseek(f, info.offset, SEEK_SET);
		*info.data=info.buf
			? info.buf
			: new(PointerFreeGC) char[to_read_size+(as_text?1:0)]; 
		*info.data_size=(size_t)read(f, *info.data, to_read_size); 

		if(ssize_t(*info.data_size)<0 || *info.data_size>to_read_size)
			throw Exception(0, 
				&file_spec, 
				"read failed: actually read %u bytes count not in [0..%u] valid range", 
					*info.data_size, to_read_size); 
	} else { // empty file
		// for both, text and binary: for text we need that terminator, for binary we need nonzero pointer to be able to save such files
		*info.data=new(PointerFreeGC) char[1]; 
		*(char*)(*info.data)=0;
		*info.data_size=0;
		return;
	}
}
File_read_result file_read(Request_charsets& charsets, const String& file_spec, 
			   bool as_text, HashStringValue *params,
			   bool fail_on_read_problem,
			   char* buf, size_t offset, size_t count) {
	File_read_result result={false, 0, 0, 0};
	if(file_spec.starts_with("http://")) {
		if(offset || count)
			throw Exception("parser.runtime",
				0,
				"offset and load options are not supported for HTTP:// file load");

		// fail on read problem
		File_read_http_result http=pa_internal_file_read_http(charsets, file_spec, as_text, params);
		result.success=true;
		result.str=http.str;
		result.length=http.length;
		result.headers=http.headers; 
	} else {
		if(params) {
			int valid_options=pa_get_valid_file_options_count(*params);
			if(valid_options!=params->count())
				throw Exception("parser.runtime",
					0,
					"invalid option passed");
		}

		File_read_action_info info={&result.str, &result.length,
			buf, offset, count}; 
		result.success=file_read_action_under_lock(file_spec, 
			"read", file_read_action, &info, 
			as_text, fail_on_read_problem); 

		if(result.length && as_text && params) {
			if( Value* vcharset_name=params->get(PA_CHARSET_NAME) ) {
				Charset asked_charset=::charsets.get(vcharset_name->as_string().
					change_case(charsets.source(), String::CC_UPPER));

				String::C body=String::C(result.str, result.length);
				body=Charset::transcode(body, asked_charset, charsets.source());

				result.str=const_cast<char *>(body.str); // hacking a little
				result.length=body.length;
			} 
		}
	}

	if(result.success && as_text) {
		// UTF-8 signature: EF BB BF
		if(result.length>=3) {
			char *in=(char *)result.str;
			if(strncmp(in, "\xEF\xBB\xBF", 3)==0) {
				result.str=in+3; result.length-=3;// skip prefix
			}
		}

		fix_line_breaks((char *)(result.str), result.length); 
	}

	return result;
}

#ifdef PA_SAFE_MODE 
void check_safe_mode(struct stat finfo, const String& file_spec, const char* fname) { 
	if(finfo.st_uid/*foreign?*/!=geteuid() 
		&& finfo.st_gid/*foreign?*/!=getegid()) 
		throw Exception("parser.runtime",  
			&file_spec,  
			"parser is in safe mode: " 
			"reading files of foreign group and user disabled " 
			"[recompile parser with --disable-safe-mode configure option], " 
			"actual filename '%s', " 
			"fuid(%d)!=euid(%d) or fgid(%d)!=egid(%d)",  
				fname, 
				finfo.st_uid, geteuid(), 
				finfo.st_gid, getegid()); 
} 
#endif 

bool file_read_action_under_lock(const String& file_spec, 
				const char* action_name, File_read_action action, void *context, 
				bool as_text, 
				bool fail_on_read_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
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
    if((f=open(fname, O_RDONLY|(as_text?_O_TEXT:_O_BINARY)))>=0) {
		try {
			if(pa_lock_shared_blocking(f)!=0)
				throw Exception("file.lock", 
						&file_spec, 
						"shared lock failed: %s (%d), actual filename '%s'", 
							strerror(errno), errno, fname);

			struct stat finfo;
			if(stat(fname, &finfo)!=0)
				throw Exception("file.missing", // hardly possible: we just opened it OK
					&file_spec, 
					"stat failed: %s (%d), actual filename '%s'", 
						strerror(errno), errno, fname);

#ifdef PA_SAFE_MODE
			check_safe_mode(finfo, file_spec, fname);
#endif

			action(finfo, f, file_spec, fname, as_text, context); 
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
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
				&file_spec, 
				"%s failed: %s (%d), actual filename '%s'", 
					action_name, strerror(errno), errno, fname);
		return false;
	}
}

void create_dir_for_file(const String& file_spec) {
	size_t pos_after=1;
	size_t pos_before;
	while((pos_before=file_spec.pos('/', pos_after))!=STRING_NOT_FOUND) {
		mkdir(file_spec.mid(0, pos_before).cstr(String::L_FILE_SPEC), 0775); 
		pos_after=pos_before+1;
	}
}

bool file_write_action_under_lock(
				const String& file_spec, 
				const char* action_name, File_write_action action, void *context, 
				bool as_text, 
				bool do_append, 
				bool do_block, 
				bool fail_on_lock_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec); 

	if((f=open(fname, 
		O_CREAT|O_RDWR
		|(as_text?_O_TEXT:_O_BINARY)
		|(do_append?O_APPEND:PA_O_TRUNC), 0664))>=0) {
		if((do_block?pa_lock_exclusive_blocking(f):pa_lock_exclusive_nonblocking(f))!=0) {
			Exception e("file.lock", 
				&file_spec, 
				"shared lock failed: %s (%d), actual filename '%s'", 
				strerror(errno), errno, fname);
			close(f); 
			if(fail_on_lock_problem)
				throw e;
			return false;
		}

		try {
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
		throw Exception(errno==EACCES?"file.access":0, 
			&file_spec, 
			"%s failed: %s (%d), actual filename '%s'", 
				action_name, strerror(errno), errno, fname);
	// here should be nothing, see rethrow above
}

#ifndef DOXYGEN
struct File_write_action_info {
	const char* str; size_t length;
}; 
#endif
static void file_write_action(int f, void *context) {
	File_write_action_info& info=*static_cast<File_write_action_info *>(context); 
	if(info.length) {
		int written=write(f, info.str, info.length); 
		if(written<0)
			throw Exception(0, 
				0, 
				"write failed: %s (%d)",  strerror(errno), errno); 
	}
}
void file_write(
				const String& file_spec, 
				const char* data, size_t size, 
				bool as_text, 
				bool do_append) {
	File_write_action_info info={data, size}; 
	file_write_action_under_lock(
		file_spec, 
		"write", file_write_action, &info, 
		as_text, 
		do_append); 
}

// throws nothing! [this is required in file_move & file_delete]
static void rmdir(const String& file_spec, size_t pos_after) {
	size_t pos_before;
	if((pos_before=file_spec.pos('/', pos_after))!=STRING_NOT_FOUND)
		rmdir(file_spec, pos_before+1); 
	
	rmdir(file_spec.mid(0, pos_after-1/* / */).cstr(String::L_FILE_SPEC)); 
}
bool file_delete(const String& file_spec, bool fail_on_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	if(unlink(fname)!=0)
		if(fail_on_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
				&file_spec, 
				"unlink failed: %s (%d), actual filename '%s'", 
					strerror(errno), errno, fname);
		else
			return false;

	rmdir(file_spec, 1); 
	return true;
}
void file_move(const String& old_spec, const String& new_spec) {
	const char* old_spec_cstr=old_spec.cstr(String::L_FILE_SPEC); 
	const char* new_spec_cstr=new_spec.cstr(String::L_FILE_SPEC); 
	
	create_dir_for_file(new_spec); 

	if(rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0, 
			&old_spec, 
			"rename failed: %s (%d), actual filename '%s' to '%s'", 
				strerror(errno), errno, old_spec_cstr, new_spec_cstr);

	rmdir(old_spec, 1); 
}


bool entry_exists(const char* fname, struct stat *afinfo) {
	struct stat lfinfo;
	bool result=stat(fname, &lfinfo)==0;
	if(afinfo)
		*afinfo=lfinfo;
	return result;
}

bool entry_exists(const String& file_spec) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	return entry_exists(fname, 0); 
}

static bool entry_readable(const String& file_spec, bool need_dir) {
	char* fname=file_spec.cstrm(String::L_FILE_SPEC); 
	if(need_dir) {
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
	if(access(fname, R_OK)==0 && entry_exists(fname, &finfo)) {
		bool is_dir=(finfo.st_mode&S_IFDIR) != 0;
		return is_dir==need_dir;
	}
	return false;
}
bool file_exist(const String& file_spec) {
	return entry_readable(file_spec, false); 
}
bool dir_exists(const String& file_spec) {
	return entry_readable(file_spec, true); 
}
const String* file_exist(const String& path, const String& name) {
	String& result=*new String(path);
	result << "/"; 
	result << name;
	return file_exist(result)?&result:0;
}
bool file_executable(const String& file_spec) {
    return access(file_spec.cstr(String::L_FILE_SPEC), X_OK)==0;
}

bool file_stat(const String& file_spec, 
			   size_t& rsize, 
			   time_t& ratime, 
			   time_t& rmtime, 
			   time_t& rctime, 
			   bool fail_on_read_problem) {
	const char* fname=file_spec.cstr(String::L_FILE_SPEC); 
	struct stat finfo;
	if(stat(fname, &finfo)!=0)
		if(fail_on_read_problem)
			throw Exception("file.missing", 
				&file_spec, 
				"getting file size failed: %s (%d), real filename '%s'", 
					strerror(errno), errno, fname);
		else
			return false;
	rsize=finfo.st_size;
	ratime=finfo.st_atime;
	rmtime=finfo.st_mtime;
	rctime=finfo.st_ctime;
	return true;
}

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

/// @todo less stupid type detection
const char* format(double value, char* fmt) {
	char local_buf[MAX_NUMBER]; 
	size_t size;
	
	if(fmt)
		if(strpbrk(fmt, "diouxX"))
			if(strpbrk(fmt, "ouxX"))
				size=snprintf(local_buf, sizeof(local_buf), fmt, (uint)value); 
			else
				size=snprintf(local_buf, sizeof(local_buf), fmt, (int)value); 
		else
			size=snprintf(local_buf, sizeof(local_buf), fmt, value); 
	else
		size=snprintf(local_buf, sizeof(local_buf), "%d", (int)value); 
	
	return pa_strdup(local_buf, size);
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

	return to_write-size;
#else
	return fwrite(buf, 1, size, stdout); 
#endif
}

char* unescape_chars(const char* cp, int len) {
	char* s=new(PointerFreeGC) char[len + 1]; 
	enum EscapeState {
		EscapeRest, 
		EscapeFirst, 
		EscapeSecond
	} escapeState=EscapeRest;
	uint escapedValue=0;
	int srcPos=0;
	int dstPos=0;
	while(srcPos < len) {
		uchar ch=(uchar)cp[srcPos]; 
		switch(escapeState) {
			case EscapeRest:
			if(ch=='%') {
				escapeState=EscapeFirst;
			} else if(ch=='+') {
				s[dstPos++]=' '; 
			} else {
				s[dstPos++]=ch;	
			}
			break;
			case EscapeFirst:
			escapedValue=hex_value[ch] << 4;
			escapeState=EscapeSecond;
			break;
			case EscapeSecond:
			escapedValue +=hex_value[ch]; 
			s[dstPos++]=(char)escapedValue;
			escapeState=EscapeRest;
			break;
		}
		srcPos++; 
	}
	s[dstPos]=0;
	return s;
}

#ifdef WIN32
void back_slashes_to_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/'; 
}
/*
void slashes_to_back_slashes(char* s) {
	if(s)
		for(; *s; s++)
			if(*s=='/')
				*s='\\'; 
}
*/
#endif

bool StrEqNc(const char* s1, const char* s2, bool strict) {
	while(true) {
		if(!(*s1)) {
			if(!(*s2))
				return true;
			else
				return !strict;
		} else if(!(*s2))
			return !strict;
		if(isalpha((unsigned char)*s1)) {
			if(tolower((unsigned char)*s1) !=tolower((unsigned char)*s2))
				return false;
		} else if((*s1) !=(*s2))
			return false;
		s1++; 
		s2++; 
	}
}

static bool isLeap(int year) {
    return !(
             (year % 4) || ((year % 400) && !(year % 100))
            ); 
}

int getMonthDays(int year, int month) {
	static int monthDays[]={
        31, 
        28, 
        31, 
        30, 
        31, 
        30, 
        31, 
        31, 
        30, 
        31, 
        30, 
        31
    }; 
	return (month == 2 && isLeap(year)) ? 29 : monthDays[month]; 
}

void remove_crlf(char* start, char* end) {
	for(char* p=start; p<end; p++)
		switch(*p) {
			case '\n': *p='|';  break;
			case '\r': *p=' ';  break;
		}
}


/// must be last in this file
#undef vsnprintf
int __vsnprintf(char* b, size_t s, const char* f, va_list l) {
	if(!s)
		return 0;

	int r;
	// note: on win32& maybe somewhere else
	// vsnprintf do not writes terminating 0 in 'buffer full' case, reducing
	--s;

	// clients do not check for negative 's', feature: ignore such prints
	if((ssize_t)s<0)
		return 0;

#if _MSC_VER
	/*
	win32: 
	mk:@MSITStore:C:\Program%20Files\Microsoft%20Visual%20Studio\MSDN\2001APR\1033\vccore.chm::/html/_crt__vsnprintf.2c_._vsnwprintf.htm

	  if the number of bytes to write exceeds buffer, then count bytes are written and Ö1 is returned
	*/
	r=_vsnprintf(b, s, f, l); 
	if(r<0) 
		r=s;
#else
	r=vsnprintf(b, s, f, l); 
	/*
	solaris: 
	man vsnprintf

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

int __snprintf(char* b, size_t s, const char* f, ...) {
	va_list l;
    va_start(l, f); 
    int r=__vsnprintf(b, s, f, l); 
    va_end(l); 
	return r;
}

/* mime64 functions are from libgmime[http://spruce.sourceforge.net/gmime/] lib */
/*
 *  Authors: Michael Zucchi <notzed@helixcode.com>
 *           Jeffrey Stedfast <fejj@helixcode.com>
 *
 *  Copyright 2000 Helix Code, Inc. (www.helixcode.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 *
 */
static char *base64_alphabet =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * g_mime_utils_base64_encode_step:
 * @in: input stream
 * @inlen: length of the input
 * @out: output string
 * @state: holds the number of bits that are stored in @save
 * @save: leftover bits that have not yet been encoded
 *
 * Base64 encodes a chunk of data. Performs an 'encode step', only
 * encodes blocks of 3 characters to the output at a time, saves
 * left-over state in state and save (initialise to 0 on first
 * invocation).
 *
 * Returns the number of bytes encoded.
 **/
static size_t
g_mime_utils_base64_encode_step (const unsigned char *in, size_t inlen, unsigned char *out, int *state, int *save)
{
	register const unsigned char *inptr;
	register unsigned char *outptr;
	
	if (inlen <= 0)
		return 0;
	
	inptr = in;
	outptr = out;
	
	if (inlen + ((unsigned char *)save)[0] > 2) {
		const unsigned char *inend = in + inlen - 2;
		register int c1 = 0, c2 = 0, c3 = 0;
		register int already;
		
		already = *state;
		
		switch (((char *)save)[0]) {
		case 1:	c1 = ((unsigned char *)save)[1]; goto skip1;
		case 2:	c1 = ((unsigned char *)save)[1];
			c2 = ((unsigned char *)save)[2]; goto skip2;
		}
		
		/* yes, we jump into the loop, no i'm not going to change it, its beautiful! */
		while (inptr < inend) {
			c1 = *inptr++;
		skip1:
			c2 = *inptr++;
		skip2:
			c3 = *inptr++;
			*outptr++ = base64_alphabet [c1 >> 2];
			*outptr++ = base64_alphabet [(c2 >> 4) | ((c1 & 0x3) << 4)];
			*outptr++ = base64_alphabet [((c2 & 0x0f) << 2) | (c3 >> 6)];
			*outptr++ = base64_alphabet [c3 & 0x3f];
			/* this is a bit ugly ... */
			if ((++already) >= 19) {
				*outptr++ = '\n';
				already = 0;
			}
		}
		
		((unsigned char *)save)[0] = 0;
		inlen = 2 - (inptr - inend);
		*state = already;
	}
	
	//d(printf ("state = %d, inlen = %d\n", (int)((char *)save)[0], inlen));
	
	if (inlen > 0) {
		register char *saveout;
		
		/* points to the slot for the next char to save */
		saveout = & (((char *)save)[1]) + ((char *)save)[0];
		
		/* inlen can only be 0 1 or 2 */
		switch (inlen) {
		case 2:	*saveout++ = *inptr++;
		case 1:	*saveout++ = *inptr++;
		}
		*(char *)save = *(char *)save+(char)inlen;
	}
	
	/*d(printf ("mode = %d\nc1 = %c\nc2 = %c\n",
		  (int)((char *)save)[0],
		  (int)((char *)save)[1],
		  (int)((char *)save)[2]));*/
	
	return (outptr - out);
}

/**
 * g_mime_utils_base64_encode_close:
 * @in: input stream
 * @inlen: length of the input
 * @out: output string
 * @state: holds the number of bits that are stored in @save
 * @save: leftover bits that have not yet been encoded
 *
 * Base64 encodes the input stream to the output stream. Call this
 * when finished encoding data with g_mime_utils_base64_encode_step to
 * flush off the last little bit.
 *
 * Returns the number of bytes encoded.
 **/
static size_t
g_mime_utils_base64_encode_close (const unsigned char *in, size_t inlen, unsigned char *out, int *state, int *save)
{
	unsigned char *outptr = out;
	int c1, c2;
	
	if (inlen > 0)
		outptr += g_mime_utils_base64_encode_step (in, inlen, outptr, state, save);
	
	c1 = ((unsigned char *)save)[1];
	c2 = ((unsigned char *)save)[2];
	
	switch (((unsigned char *)save)[0]) {
	case 2:
		outptr[2] = base64_alphabet [(c2 & 0x0f) << 2];
		goto skip;
	case 1:
		outptr[2] = '=';
	skip:
		outptr[0] = base64_alphabet [c1 >> 2];
		outptr[1] = base64_alphabet [c2 >> 4 | ((c1 & 0x3) << 4)];
		outptr[3] = '=';
		outptr += 4;
		break;
	}
	
	*outptr++ = 0;
	
	*save = 0;
	*state = 0;
	
	return (outptr - out);
}

static unsigned char gmime_base64_rank[256] = {
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};

/**
 * g_mime_utils_base64_decode_step:
 * @in: input stream
 * @inlen: max length of data to decode
 * @out: output stream
 * @state: holds the number of bits that are stored in @save
 * @save: leftover bits that have not yet been decoded
 *
 * Decodes a chunk of base64 encoded data.
 *
 * Returns the number of bytes decoded (which have been dumped in @out).
 **/
size_t
g_mime_utils_base64_decode_step (const unsigned char *in, size_t inlen, unsigned char *out, int *state, int *save)
{
	const unsigned char *inptr;
	unsigned char *outptr;
	const unsigned char *inend;
	int saved;
	unsigned char c;
	int i;
	
	inend = in + inlen;
	outptr = out;
	
	/* convert 4 base64 bytes to 3 normal bytes */
	saved = *save;
	i = *state;
	inptr = in;
	while (inptr < inend) {
		c = gmime_base64_rank[*inptr++];
		if (c != 0xff) {
			saved = (saved << 6) | c;
			i++;
			if (i == 4) {
				*outptr++ = (unsigned char)(saved >> 16);
				*outptr++ = (unsigned char)(saved >> 8);
				*outptr++ = (unsigned char)(saved);
				i = 0;
			}
		}
	}
	
	*save = saved;
	*state = i;
	
	/* quick scan back for '=' on the end somewhere */
	/* fortunately we can drop 1 output char for each trailing = (upto 2) */
	i = 2;
	while (inptr > in && i) {
		inptr--;
		if (gmime_base64_rank[*inptr] != 0xff) {
			if (*inptr == '=' && outptr > out)
				outptr--;
			i--;
		}
	}
	
	/* if i != 0 then there is a truncation error! */
	return (outptr - out);
}


char* pa_base64_encode(const char *in, size_t in_size)
{
	/* wont go to more than 2x size (overly conservative) */
	char* result=new(PointerFreeGC) char[in_size * 2 + 6];
	int state=0;
	int save=0;
#ifndef NDEBUG
	size_t filled=
#endif
		g_mime_utils_base64_encode_close ((const unsigned char*)in, in_size, 
		(unsigned char*)result, &state, &save);
	assert(filled <= in_size * 2 + 6);

	return result;
}


char* pa_base64_encode(const String& file_spec)
{
	unsigned char* base64=0;
	File_base64_action_info info={&base64}; 

	file_read_action_under_lock(file_spec, 
		"pa_base64_encode", file_base64_file_action, &info);

	return (char*)base64; 
}


static void file_base64_file_action(
			     struct stat& finfo, 
			     int f, 
			     const String&, const char* /*fname*/, bool, 
			     void *context) {

	if(finfo.st_size) { 
		File_base64_action_info& info=*static_cast<File_base64_action_info *>(context);
		*info.base64=new(PointerFreeGC) unsigned char[finfo.st_size * 2 + 6]; 
		unsigned char* base64 = *info.base64;
		int state=0;
		int save=0;
		int nCount;
		do {
			unsigned char buffer[FILE_BUFFER_SIZE];
			nCount = file_block_read(f, buffer, sizeof(buffer));
			if( nCount ){
				size_t filled=g_mime_utils_base64_encode_step ((const unsigned char*)buffer, nCount, base64, &state, &save);
				base64+=filled;
			}
		} while(nCount > 0);
		g_mime_utils_base64_encode_close (0, 0, base64, &state, &save);
	}
}

void pa_base64_decode(const char *in, size_t in_size, char*& result, size_t& result_size)
{
	/* wont go to more than had (overly conservative) */
	result=new(PointerFreeGC) char[in_size+1/*terminator*/];
	int state=0;
	int save=0;
	result_size=
		g_mime_utils_base64_decode_step ((const unsigned char*)in, in_size, 
		(unsigned char*)result, &state, &save);
	assert(result_size <= in_size);
	result[result_size]=0; // for text files
}


int file_block_read(const int f, unsigned char* buffer, const size_t size){
	int nCount = read(f, buffer, size);
	if (nCount < 0)
		throw Exception(0, 
			0, 
			"read failed: %s (%d)",  strerror(errno), errno); 
	return nCount;
}

const unsigned long pa_crc32(const char *in, size_t in_size)
{
	unsigned long crc32=0xFFFFFFFF;

		InitCrc32Table();
		for(size_t i = 0; i < in_size; i++) CalcCrc32(in[i], crc32);

	return ~crc32; 
}

const unsigned long pa_crc32(const String& file_spec)
{
	unsigned long crc32=0xFFFFFFFF;
	file_read_action_under_lock(file_spec, "crc32", file_crc32_file_action, &crc32);
	return ~crc32; 
}

static void file_crc32_file_action(
			     struct stat& finfo, 
			     int f, 
			     const String&, const char* /*fname*/, bool, 
			     void *context)
{
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

