/** @file
	Parser: commonly functions.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_common.C,v 1.90 2001/11/14 13:07:41 paf Exp $
*/

#include "pa_common.h"
#include "pa_types.h"
#include "pa_exception.h"
#include "pa_pool.h"
#include "pa_globals.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_string.h"

#ifndef _O_TEXT
#	define _O_TEXT 0
#endif
#ifndef _O_BINARY
#	define _O_BINARY 0
#endif
#ifndef O_TRUNC
#	define O_TRUNC 0
#endif

int __vsnprintf(char *b, size_t s, const char *f, va_list l) {
	if(!s)
		return 0;

	int r;
	// note: on win32& maybe somewhere else
	// vsnprintf do not writes terminating 0 in 'buffer full' case, reducing
	--s;
#if _MSC_VER
	/*
	win32: 
	mk:@MSITStore:C:\Program%20Files\Microsoft%20Visual%20Studio\MSDN\2001APR\1033\vccore.chm::/html/_crt__vsnprintf.2c_._vsnwprintf.htm

	  if the number of bytes to write exceeds buffer, then count bytes are written and –1 is returned
	*/
	r=_vsnprintf(b, s, f, l);
	if(r<0) 
		r=s;
#else
	r=_vsnprintf(b, s, f, l);
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
	else if(r>s)
		r=s;
#endif
	b[r]=0;
	return r;
}

int __snprintf(char *b, size_t s, const char *f, ...) {
	va_list l;
    va_start(l, f);
    int r=__vsnprintf(b, s, f, l);
    va_end(l);
	return r;
}

static char *strnchr(char *buf, size_t size, char c) {
	for(; size-->0; buf++) {
		if(*buf==c)
			return buf;
	}

	return 0;
}

void fix_line_breaks(char *buf, size_t& size) {
	//_asm int 3;
	const char * const eob=buf+size;
	char *dest=buf;
	// fix DOS: \r\n -> \n
	// fix Macintosh: \r -> \n
	char *bol=buf;
	while(char *eol=strnchr(bol, eob -bol, '\r')) {
		size_t len=eol-bol;
		if(dest!=bol)
			memcpy(dest, bol, len);
		dest+=len;
		*dest++='\n';

		if(&eol[1]<eob && eol[1]=='\n') { // \r,\n = DOS
			bol=eol+2;
			size--;
		} else // \r,not \n = Macintosh
			bol=eol+1;
	}
	// last piece without \r, including terminating 0
	if(dest!=bol)
		memcpy(dest, bol, eob-bol);
}

char *file_read_text(Pool& pool, const String& file_spec, bool fail_on_read_problem) {
	void *result;  size_t size;
	return file_read(pool, file_spec, result, size, true, fail_on_read_problem)?(char *)result:0;
}
bool file_read(Pool& pool, const String& file_spec, 
			   void*& data, size_t& data_size, bool as_text,
			   bool fail_on_read_problem,
			   size_t offset, size_t limit) {
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
//printf("file_read(%s)\n", fname);
	int f;
    struct stat finfo;

	// first open, next stat:
	// directory update of NTFS hard links performed on open.
	// ex: 
	//   a.html:^test[] and b.html hardlink to a.html
	//   user inserts ! before ^test in a.html
	//   directory entry of b.html in NTFS not updated at once,
	//   they delay update till open, so we would receive "!^test[" string
	//   if would do stat, next open.
    if(
		(f=open(fname, O_RDONLY|(as_text?_O_BINARY/*_O_TEXT*/:_O_BINARY)))>=0 && 
		stat(fname, &finfo)==0) {
		/*if(exclusive)
			flock(f, LOCK_EX);*/
		size_t max_size=limit?min(offset+limit, finfo.st_size)-offset:finfo.st_size;
		int read_size;
		if(!max_size) { // eof
			if(as_text) {
				data=pool.malloc(1);
				*(char*)data=0;
			} else 
				data=0;
			read_size=0;
		} else {
			data=pool.malloc(max_size+(as_text?1:0), 3);
			if(offset)
				lseek(f, offset, SEEK_SET);
			read_size=read(f, data, max_size);
		}
		/*if(exclusive)
			flock(f, LOCK_UN);*/
		close(f);
		if(!max_size) // eof
			return true;

		if(read_size<0 || read_size>max_size)
			throw Exception(0, 0, 
				&file_spec, 
				"read failed: actually read %d bytes count not in [0..%lu] valid range", 
					read_size, (unsigned long)max_size); //never

		data_size=read_size;		
		if(as_text) {
			fix_line_breaks((char *)data, data_size);
			// note: after fixing
			((char*&)data)[data_size]=0;
		}
		return true;
    }
	if(fail_on_read_problem)
		throw Exception(0, 0, 
			&file_spec, 
			"read failed: %s (%d), actual filename '%s'", 
				strerror(errno), errno, fname);
    return false;
}

static void create_dir_for_file(const String& file_spec) {
	size_t pos_after=1;
	int pos_before;
	while((pos_before=file_spec.pos("/", 1, pos_after))>=0) {
		mkdir(file_spec.mid(0, pos_before).cstr(String::UL_FILE_SPEC), 0775);
		pos_after=pos_before+1;
	}
}

void file_write(Pool& pool, 
				const String& file_spec, 
				const void *data, size_t size, 
				bool as_text,
				bool do_append/*, 
				bool exclusive*/) {
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec);

	if((f=open(fname, 
		O_CREAT|O_RDWR
		|(as_text?_O_TEXT:_O_BINARY)
		|(do_append?O_APPEND:O_TRUNC), 0666))>=0) {
		/*if(exclusive)
			flock(f, LOCK_EX);*/
		
		if(size) write(f, data, size);
#if O_TRUNC==0
		ftruncate(f, size);
#endif
		/*if(exclusive)
			flock(f, LOCK_UN);*/
		close(f);
	} else
		throw Exception(0, 0, 
			&file_spec, 
			"write failed: %s (%d), actual filename '%s'", 
				strerror(errno), errno, fname);
}

// throws nothing! [this is required in file_move & file_delete]
static void rmdir(const String& file_spec, size_t pos_after) {
	int pos_before;
	if((pos_before=file_spec.pos("/", 1, pos_after))>=0)
		rmdir(file_spec, pos_before+1);
	
	rmdir(file_spec.mid(0, pos_after-1/* / */).cstr(String::UL_FILE_SPEC));
}
void file_delete(Pool& pool, const String& file_spec) {
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	if(unlink(fname)!=0)
		throw Exception(0, 0, 
			&file_spec, 
			"unlink failed: %s (%d), actual filename '%s'", 
				strerror(errno), errno, fname);

	rmdir(file_spec, 1);
}
void file_move(Pool& pool, const String& old_spec, const String& new_spec) {
	const char *old_spec_cstr=old_spec.cstr(String::UL_FILE_SPEC);
	const char *new_spec_cstr=new_spec.cstr(String::UL_FILE_SPEC);
	
	create_dir_for_file(new_spec);

	if(rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(0, 0, 
			&old_spec, 
			"rename failed: %s (%d), actual filename '%s' to '%s'", 
				strerror(errno), errno, old_spec_cstr, new_spec_cstr);

	rmdir(old_spec, 1);
}


static bool entry_readable(const String& file_spec, bool need_dir) {
    const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	struct stat finfo;
	if(access(fname, R_OK)==0 && stat(fname, &finfo)==0) {
		bool is_dir=finfo.st_mode&S_IFDIR != 0;
		return is_dir==need_dir;
	}
	return false;
}
bool file_readable(const String& file_spec) {
	return entry_readable(file_spec, false);
}
bool dir_readable(const String& file_spec) {
	return entry_readable(file_spec, true);
}
String *file_readable(const String& path, const String& name) {
	String *result=new(path.pool()) String(path);
	*result << "/";
	*result << name;
	return file_readable(*result)?result:0;
}
bool file_executable(const String& file_spec) {
    return access(file_spec.cstr(String::UL_FILE_SPEC), X_OK)==0;
}

bool file_stat(const String& file_spec, 
			   size_t& rsize, 
			   time_t& ratime,
			   time_t& rmtime,
			   time_t& rctime,
			   bool fail_on_read_problem) {
	Pool& pool=file_spec.pool();
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
    struct stat finfo;
	if(stat(fname, &finfo)!=0)
		if(fail_on_read_problem)
			throw Exception(0, 0, 
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

char *getrow(char **row_ref, char delim) {
    char *result=*row_ref;
    if(result) {
		*row_ref=strchr(result, delim);
		if(*row_ref) 
			*((*row_ref)++)=0; 
		else if(!*result) 
			return 0;
    }
    return result;
}

char *lsplit(char *string, char delim) {
    if(string) {
		char *v=strchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return 0;
}

char *lsplit(char **string_ref, char delim) {
    char *result=*string_ref;
	char *next=lsplit(*string_ref, delim);
    *string_ref=next;
    return result;
}

char *rsplit(char *string, char delim) {
    if(string) {
		char *v=strrchr(string, delim);
		if(v) {
			*v=0;
			return v+1;
		}
    }
    return NULL;	
}

/// @todo less stupid type detection
char *format(Pool& pool, double value, char *fmt) {
	char *result=(char *)pool.malloc(MAX_NUMBER, 4);
	if(fmt)
		if(strpbrk(fmt, "diouxX"))
			if(strpbrk(fmt, "ouxX"))
				snprintf(result, MAX_NUMBER, fmt, (uint)value );
			else
				snprintf(result, MAX_NUMBER, fmt, (int)value );
		else
			snprintf(result, MAX_NUMBER, fmt, value);
	else
		snprintf(result, MAX_NUMBER, "%d", (int)value);
	
	return result;
}

size_t stdout_write(const void *buf, size_t size) {
#ifdef WIN32
	do{
		int chunk_written=fwrite(buf, 1, min(8*0x400, size), stdout);
		if(chunk_written<=0)
			break;
		size-=chunk_written;
		buf=((const char*)buf)+chunk_written;
	} while(size>0);

	return size;
#else
	return fwrite(buf, 1, size, stdout);
#endif
}

char *unescape_chars(Pool& pool, const char *cp, int len) {
	char *s=(char *)pool.malloc(len + 1, 5);
	enum EscapeState {
		EscapeRest, 
		EscapeFirst, 
		EscapeSecond
	} escapeState=EscapeRest;
	int escapedValue=0;
	int srcPos=0;
	int dstPos=0;
	while(srcPos < len) {
		int ch=cp[srcPos];
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
			s[dstPos++]=escapedValue;
			escapeState=EscapeRest;
			break;
		}
		srcPos++;
	}
	s[dstPos]=0;
	return s;
}

/// used by attributed_meaning_to_string / append_attribute_subattribute
struct Attributed_meaning_info {
	String *header; // header line being constructed
	String::Untaint_lang lang; // language in which to append to that line
};
static void append_attribute_subattribute(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	if(akey==VALUE_NAME)
		return;

	Attributed_meaning_info& ami=*static_cast<Attributed_meaning_info *>(info);

	// ...; charset=windows1251
	*ami.header << "; ";
	ami.header->append(akey, ami.lang);
	*ami.header << "=";
	ami.header->append(static_cast<Value *>(avalue)->as_string(), ami.lang);
}
const String& attributed_meaning_to_string(Value& meaning, 
										   String::Untaint_lang lang) {
	String &result=*new(meaning.pool()) String(meaning.pool());
	if(Hash *hash=meaning.get_hash(0)) {
		// $value(value) $subattribute(subattribute value)
		if(Value *value=static_cast<Value *>(hash->get(*value_name)))
			result.append(value->as_string(), lang, true);

		Attributed_meaning_info attributed_meaning_info={
			&result,
			lang
		};
		hash->for_each(append_attribute_subattribute, &attributed_meaning_info);
	} else // result value
		result.append(meaning.as_string(), lang, true);

	return result;
}

#ifdef WIN32
void back_slashes_to_slashes(char *s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/';
}
/*
void slashes_to_back_slashes(char *s) {
	if(s)
		for(; *s; s++)
			if(*s=='/')
				*s='\\';
}
*/
#endif

bool StrEqNc(const char *s1, const char *s2, bool strict) {
	while(true) {
		if(!(*s1)) {
			if(!(*s2))
				return true;
			else
				return !strict;
		} else if(!(*s2))
			return !strict;
		if(isalpha(*s1)) {
			if(tolower(*s1) !=tolower(*s2))
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
    int monthDays[]={
        31,
        isLeap(year) ? 29 : 28,
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
    return monthDays[month];
}

void remove_crlf(char *start, char *end) {
	for(char *p=start; p<end; p++)
		switch(*p) {
			case '\n': *p='|'; break;
			case '\r': *p=' '; break;
		}
}
