/** @file
	Parser: commonly used functions.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_common.h,v 1.57 2001/10/30 16:30:08 paf Exp $
*/

#ifndef PA_COMMON_H
#define PA_COMMON_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_string.h"

class Value;

#if _MSC_VER
/*
inline int open( const char *filename, int oflag ) { return _open(filename, oflag); }
inline int close( int handle ) { return _close(handle); }
inline int read( int handle, void *buffer, unsigned int count ) { return _read(handle,buffer,count); }
inline int write( int handle, const void *buffer, unsigned int count ) { return _write(handle,buffer,count); }
inline int stat( const char *path, struct _stat *buffer ) { return _stat(path, buffer); }
inline long lseek( int handle, long offset, int origin ) { return _lseek(handle, offset, origin); }
*/

#ifndef vsnprintf
int __vsnprintf(char *, size_t, const char *, va_list);
#	define vsnprintf __vsnprintf 
#endif
#ifndef snprintf
int __snprintf(char *, size_t, const char *, ...);
#	define snprintf __snprintf
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

/** under WIN32 "t" mode fixes DOS chars OK, 
	can't say that about other systems/ line break styles
*/
void fix_line_breaks(
					 char *cstr,  ///< nevertheless must be 0 ended
					 size_t& size ///< used to speedup next actions
					 );

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
			   bool fail_on_read_problem=true,
			   size_t offset=0, size_t limit=0);

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
/**
	move specified file 
	throws an exception in case of problems
*/
void file_move(Pool& pool, const String& old_spec, const String& new_spec);

bool file_readable(const String& file_spec);
bool dir_readable(const String& file_spec);
String *file_readable(const String& path, const String& name);
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
//char *lsplit(char *string, char delim);
char *lsplit(char **string_ref,char delim);
char *rsplit(char *string, char delim);
char *format(Pool& pool, double value, char *fmt);

#ifndef max
inline int max(int a,int b) { return a>b?a:b; }
inline int min(int a,int b){ return a<b?a:b; }
#endif

size_t stdout_write(const void *buf, size_t size);

char *unescape_chars(Pool& pool, const char *cp, int len);

/**
	$content-type[text/html] -> 
		content-type: text/html
	$content-type[$value[text/html] charset[windows-1251]] -> 
		content-type: text/html; charset=windows-1251
*/
const String& attributed_meaning_to_string(Value& meaning, String::Untaint_lang lang);

#ifdef WIN32
void back_slashes_to_slashes(char *s);
//void slashes_to_back_slashes(char *s);
#endif

#ifndef _qsort
#	define _qsort(names,cnt,sizeof_names,func_addr) \
		qsort(names,cnt,sizeof_names,func_addr)
#endif

bool StrEqNc(const char *s1, const char *s2, bool strict=true);

#define SECS_PER_DAY (60*60*24)
int getMonthDays(int year, int month);

void remove_crlf(char *start, char *end);

#endif
