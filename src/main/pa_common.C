/*
	Parser
	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_common.C,v 1.10 2001/03/12 21:54:20 paf Exp $
*/

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

#include "pa_common.h"
#include "pa_types.h"
#include "pa_exception.h"

#ifdef WIN32

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

char *file_read(Pool& pool, const char *fname, bool fail_on_read_problem) {
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
		POOL_THROW(0,0,
			0,
			"use: can not read '%s' file", fname);
    return 0;
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

char *lsplit(char *string_ref, char delim) {
    if(string_ref) {
		char *v=strchr(string_ref, delim);
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
    if(string){
		char *v=strrchr(string, delim);
		if (v){
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
				snprintf(result, MAX_NUMBER, fmt, (uint)value );
			else
				snprintf(result, MAX_NUMBER, fmt, (int)value );
		else
			snprintf(result, MAX_NUMBER, fmt, value);
	else
		snprintf(result, MAX_NUMBER, "%d", (int)value);
	
	return result;
}
