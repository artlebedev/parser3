
/** @file
	Parser: commonly functions.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_common.C,v 1.43 2001/04/10 07:40:50 paf Exp $
*/

#include "pa_config_includes.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "pa_common.h"
#include "pa_types.h"
#include "pa_exception.h"
#include "pa_pool.h"
#include "pa_globals.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_string.h"

#if _MSC_VER

int __vsnprintf(char *b, size_t s, const char *f, va_list l) {
	int r=_vsnprintf(b, --s, f, l);
	b[s]=0;
	return r;
}
int __snprintf(char *b, size_t s, const char *f, ...) {
	va_list l;
    va_start(l, f);
    int r=__vsnprintf(b, s, f, l);
    va_end(l);
	return r;
}

#endif


char *file_read_text(Pool& pool, const String& file_spec, bool fail_on_read_problem) {
	void *result;
	size_t size;
	return file_read(pool, file_spec, result, size, true, 
		fail_on_read_problem)?(char *)result:0;
}
bool file_read(Pool& pool, const String& file_spec, 
			   void*& data, size_t& size, bool as_text,
			   bool fail_on_read_problem) {
	char *fname=file_spec.cstr(String::UL_FILE_NAME);
	int f;
    struct stat finfo;

	// first open, next stat:
	// directory update of NTFS symbolic links performed on open.
	// ex: 
	//   a.html:^test[] and b.html hardlink to a.html
	//   user inserts ! before ^test in a.html
	//   directory entry of b.html in NTFS not updated at once,
	//   they delay update till open, so we would receive "!^test[" string
	//   if would do stat, next open.
    if(
		(f=open(fname, O_RDONLY|(as_text?_O_TEXT:_O_BINARY)))>=0 && 
		stat(fname, &finfo)==0) {
		/*if(exclusive)
			flock(f, LOCK_EX);*/
		data=pool.malloc(finfo.st_size+(as_text?1:0));
		size=read(f, data, finfo.st_size);
		/*if(exclusive)
			flock(f, LOCK_UN);*/
		close(f);

		if(size>=0 && size<=(size_t)finfo.st_size) {
			if(as_text)
				((char *)data)[size]=0;
		} else
			PTHROW(0, 0, 
				&file_spec, 
				"read failed: actually read %d bytes count not in [0..%ul] valid range", 
					size, (unsigned long)finfo.st_size); //never
		
		return true;//prepare_config(result, remove_empty_lines);
    }
	if(fail_on_read_problem)
		PTHROW(0, 0, 
			&file_spec, 
			"read failed: %s (#%d)", strerror(errno), errno);
    return false;
}

/// @test mkdirs
void file_write(Pool& pool, 
				const String& file_spec, 
				const void *data, size_t size, 
				bool as_text/*, 
				bool exclusive*/) {
	char *fname=file_spec.cstr(String::UL_FILE_NAME);
	int f;
	if(access(fname, F_OK)!=0) {/*no*/
		if((f=open(fname, O_WRONLY|O_CREAT|_O_BINARY, 0666))>0)
			close(f);
	}
	if(access(fname, R_OK|W_OK)==0) {
		int mode=O_RDWR|(as_text?_O_TEXT:_O_BINARY)
#ifdef WIN32
			|O_TRUNC
#endif
		;
		if((f=open(fname, mode, 0666))>=0) {
			/*if(exclusive)
				flock(f, LOCK_EX);*/
			
			if(size) write(f, data, size);
#ifndef WIN32
			ftruncate(f, size);
#endif
			/*if(exclusive)
				flock(f, LOCK_UN);*/
			close(f);
			return;
		}
	}
	PTHROW(0, 0, 
		&file_spec, 
		"write failed: %s (#%d)", strerror(errno), errno);
}

void file_delete(Pool& pool, const String& file_spec) {
	if(unlink(file_spec.cstr(String::UL_FILE_NAME))!=0)
		PTHROW(0, 0, 
			&file_spec, 
			"unlink failed: %s (#%d)", strerror(errno), errno);
}

bool file_readable(const String& file_spec) {
    return access(file_spec.cstr(String::UL_FILE_NAME), R_OK)==0;
}
bool file_executable(const String& file_spec) {
    return access(file_spec.cstr(String::UL_FILE_NAME), X_OK)==0;
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
	char *result=(char *)pool.malloc(MAX_NUMBER);
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

const char *unescape_chars(Pool& pool, const char *cp, int len) {
	char *s=(char *)pool.malloc(len + 1);
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

struct Attributed_meaning_info {
	String::Untaint_lang lang;
	String *header;
};
static void append_attribute_subattribute(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	if(akey==VALUE_NAME)
		return;

	Attributed_meaning_info& ami=*static_cast<Attributed_meaning_info *>(info);

	// ...; charset=windows1251
	if(ami.header->size())
		ami.header->APPEND_CONST("; ");
	ami.header->append(akey, ami.lang, true);
	ami.header->APPEND_CONST("=");
	ami.header->append(static_cast<Value *>(avalue)->as_string(), 
		ami.lang, true);
}
const String& attributed_meaning_to_string(Value& meaning, String::Untaint_lang lang) {
	String &result=*new(meaning.pool()) String(meaning.pool());
	if(Hash *hash=meaning.get_hash()) {
		// $value(value) $subattribute(subattribute value)
		if(Value *value=static_cast<Value *>(hash->get(*value_name)))
			result.append(value->as_string(), lang, true);

		Attributed_meaning_info attributed_meaning_info={
			lang,
			&result
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
