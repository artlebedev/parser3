/** @file
	Parser: commonly functions.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_common.C,v 1.25 2001/03/24 10:54:46 paf Exp $
*/

#include "pa_config_includes.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <errno.h>

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


char *file_read_text(Pool& pool, const char *fname, bool fail_on_read_problem) {
    int f;
    struct stat finfo;
    if(fname && !stat(fname,&finfo) &&(f=open(fname,O_RDONLY
#ifdef WIN32
		|O_TEXT
#endif
		))>=0) {
		/*if(exclusive)
			flock(f, LOCK_EX);*/

		char *result=(char *)pool.malloc(finfo.st_size+1);
		int read_size=read(f,result,finfo.st_size);
		if(read_size>=0 && read_size<=finfo.st_size) 
			result[read_size]='\0';
		/*if(exclusive)
			flock(f, LOCK_UN);*/
		close(f);
		return result;//prepare_config(result, remove_empty_lines);
    }
	if(fail_on_read_problem)
		PTHROW(0,0,
			0,
			"can not read '%s' file", fname);
    return 0;
}

void file_write(Pool& pool, 
				const char *fname, 
				const char *data, size_t size, 
				bool as_text/*,
				bool exclusive*/) {
	if(fname) {
		int f;
		if(access(fname, F_OK)!=0) {/*no*/
			if((f=open(fname,O_WRONLY|O_CREAT|_O_BINARY,0666))>0)
				close(f);
		}
		if(access(fname, R_OK|W_OK)==0) {
			int mode=O_RDWR
#ifdef WIN32
				|O_TRUNC
#endif
			;
			mode|=as_text?_O_TEXT:_O_BINARY;
			if((f=open(fname,mode,0666))>=0) {
				/*if(exclusive)
					flock(f, LOCK_EX);*/
				
				if(size) write(f,data,size);
#ifndef WIN32
				ftruncate(f,size);
#endif
				/*if(exclusive)
					flock(f, LOCK_UN);*/
				close(f);
				return;
			}
		}
	}
	if(fname)
		PTHROW(0, 0,
			0,
			"file_write('%s'): %s (#%d)", 
				fname, strerror(errno), errno);
	else
		PTHROW(0, 0,
			0,
			"file_write: no filename specified");
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

char *format(Pool& pool, double value, char *fmt) {
	char *result=(char *)pool.malloc(MAX_NUMBER);
	if(fmt)
		if(strpbrk(fmt, "diouxX"))
			if(strpbrk(fmt, "ouxX"))
				snprintf(result, MAX_NUMBER, fmt,(uint)value );
			else
				snprintf(result, MAX_NUMBER, fmt,(int)value );
		else
			snprintf(result, MAX_NUMBER, fmt, value);
	else
		snprintf(result, MAX_NUMBER, "%d",(int)value);
	
	return result;
}

size_t stdout_write(const char *buf, size_t size) {
#ifdef WIN32
	do{
		int chunk_written=fwrite(buf, 1, min(8*0x400, size), stdout);
		if(chunk_written<=0)
			break;
		size-=chunk_written;
		buf+=chunk_written;
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

static void append_attribute_subattribute(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	if(akey==VALUE_NAME)
		return;

	// ...; charset=windows1251
	String *string=static_cast<String *>(info);
	string->APPEND_CONST("; ");
	string->append(akey, String::UL_HEADER, true);
	string->APPEND_CONST("=");
	string->append(static_cast<Value *>(avalue)->as_string(), 
		String::UL_HEADER, true);
}
const String& attributed_meaning_to_string(Value& meaning) {
	String &result=*new(meaning.pool()) String(meaning.pool());
	if(Hash *hash=meaning.get_hash()) {
		// $value(value) $subattribute(subattribute value)
		if(Value *value=static_cast<Value *>(hash->get(*value_name)))
			result.append(value->as_string(), String::UL_HEADER, true);

		hash->for_each(append_attribute_subattribute, &result);
	} else // result value
		result.append(meaning.as_string(), String::UL_HEADER, true);

	return result;
}

#ifdef WIN32
void back_slashes_to_slashes(char *s) {
	if(s)
		for(; *s; s++)
			if(*s=='\\')
				*s='/';
}
#endif
