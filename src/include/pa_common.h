/** @file
	Parser: commonly used functions.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_common.h,v 1.29 2001/03/28 09:38:07 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#include "pa_config_includes.h"
#include <stdio.h>

#include "pa_pool.h"

class String;
class Value;

#if _MSC_VER

#ifndef open
#	define open _open
#endif
#ifndef close
#	define close _close
#endif
#ifndef read
#	define read _read
#endif
#ifndef write
#	define write _write
#endif
#ifndef stat
#	define stat _stat
#endif

#ifndef vsnprintf
#	define vsnprintf __vsnprintf 
int __vsnprintf(char *, size_t, const char *, va_list);
#endif
#ifndef snprintf
#	define snprintf __snprintf
int __snprintf(char *, size_t, const char *, ...);
#endif

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

/**
	read specified text file using pool, 
	if fail_on_read_problem is true[default] throws an exception
*/
char *file_read_text(Pool& pool, 
					 const String& file_spec, 
					 bool fail_on_read_problem=true);

/**
	read specified file using pool, 
	if fail_on_read_problem is true[default] throws an exception
*/
bool file_read(Pool& pool, const String& file_spec, 
			   void*& data, size_t& size, 
			   bool as_text,
			   bool fail_on_read_problem=true);

/**
	write data to specified file using pool, 
	throws an exception in case of problems
*/
void file_write(Pool& pool, 
				const String& file_spec,
				const void *data, size_t size, 
				bool as_text/*,
				bool exclusive=false*/);

/**
	delete specified file 
	throws an exception in case of problems
*/
void file_delete(Pool& pool, const String& file_spec);

bool file_readable(const String& file_spec);

/**
	scans for @a delim[default \n] in @a *row_ref, 
	@return piece of line before it or end of string, if no @a delim found
	assigns @a *row_ref to point right after delimiter if there were one
	or to zero if no @a delim were found.
*/
char *getrow(char **row_ref,char delim='\n');
//char *lsplit(char *string, char delim);
char *lsplit(char **string_ref,char delim);
char *rsplit(char *string, char delim);
char *format(Pool& pool, double value, char *fmt);

#ifndef max
inline int max(int a,int b) { return a>b?a:b; }
inline int min(int a,int b){ return a<b?a:b; }
#endif

size_t stdout_write(const char *buf, size_t size);

const char *unescape_chars(Pool& pool, const char *cp, int len);

/**
	$content-type[text/html] -> 
		content-type: text/html
	$content-type[$value[text/html] charset[windows-1251]] -> 
		content-type: text/html; charset=windows-1251
*/
const String& attributed_meaning_to_string(Value& meaning);


#ifdef WIN32
void back_slashes_to_slashes(char *s);
#endif

#ifndef _qsort
#	define _qsort(names,cnt,sizeof_names,func_addr) \
		qsort(names,cnt,sizeof_names,func_addr)
#endif

#endif
