/** @file
	Parser: commonly functions.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_COMMON_C="$Date: 2002/09/18 12:40:38 $";

#include "pa_common.h"
#include "pa_exception.h"
#include "pa_globals.h"

#ifdef WIN32
#	include <windows.h>
#endif

// some maybe-undefined constants

#ifndef _O_TEXT
#	define _O_TEXT 0
#endif
#ifndef _O_BINARY
#	define _O_BINARY 0
#endif
#ifndef O_TRUNC
#	define O_TRUNC 0
#endif

// locking constants

#ifdef HAVE_FLOCK

static int lock_shared_blocking(int fd) { return flock(fd, LOCK_SH); }
static int lock_exclusive_blocking(int fd) { return flock(fd, LOCK_EX); }
static int lock_exclusive_nonblocking(int fd) { return flock(fd, LOCK_EX || LOCK_NB); }
static int unlock(int fd) { return flock(fd, LOCK_UN); }

#else
#ifdef HAVE__LOCKING

#define FLOCK(operation) lseek(fd, 0, SEEK_SET); return _locking(fd, operation, 1)
static int lock_shared_blocking(int fd) { FLOCK(_LK_LOCK); }
static int lock_exclusive_blocking(int fd) { FLOCK(_LK_LOCK); }
static int lock_exclusive_nonblocking(int fd) { FLOCK(_LK_NBLCK); }
static int unlock(int fd) { FLOCK(_LK_UNLCK); }

#else
#ifdef HAVE_FCNTL

#define FLOCK(cmd, arg) struct flock ls={arg, SEEK_SET}; return fcntl(fd, cmd, &ls)
static int lock_shared_blocking(int fd) { FLOCK(F_SETLKW, F_RDLCK); }
static int lock_exclusive_blocking(int fd) { FLOCK(F_SETLKW, F_WRLCK); }
static int lock_exclusive_nonblocking(int fd) { FLOCK(F_SETLK, F_RDLCK); }
static int unlock(int fd) { FLOCK(F_SETLK, F_UNLCK); }

#else
#ifdef HAVE_LOCKF

#define FLOCK(fd, operation) lseek(fd, 0, SEEK_SET); return lockf(fd, operation, 1)
static int lock_shared_blocking(int fd) { FLOCK(F_LOCK); } // on intel solaris man doesn't have doc on shared blocking
static int lock_exclusive_blocking(int fd) { FLOCK(F_LOCK); }
static int lock_exclusive_nonblocking(int fd) { FLOCK(F_TLOCK); }
static int unlock(int fd) { FLOCK(F_TLOCK); }

#else

#error unable to find file locking func

#endif
#endif
#endif
#endif

static char *strnchr(char *buf, size_t size, char c) {
	// sanity check
	if(!buf)
		return 0;

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
    if((f=open(fname, O_RDONLY|(as_text?_O_TEXT:_O_BINARY)))>=0) {
		if(lock_shared_blocking(f)!=0) {
			Exception e("file.lock",
					&file_spec, 
					"shared lock failed: %s (%d), actual filename '%s'", 
						strerror(errno), errno, fname);
			unlock(f);
			close(f);
			if(fail_on_read_problem)
				throw e;
			return false;
		}
		if(stat(fname, &finfo)!=0) {
			Exception e("file.missing",
					&file_spec, 
					"stat failed: %s (%d), actual filename '%s'", 
						strerror(errno), errno, fname);
			unlock(f);
			close(f);
			if(fail_on_read_problem)
				throw e;
			return false;
		}
#ifdef NO_FOREIGN_GROUP_FILES
		if(finfo.st_gid/*foreign?*/!=getegid()) {
			Exception e("parser.runtime",
				&file_spec,
				"parser reading files of foreign group disabled [recompile parser without --disable-foreign-group-files configure option], actual filename '%s'", 
					fname);
			unlock(f);
			close(f);
			if(fail_on_read_problem)
				throw e;
			return false;
		}
#endif
		size_t max_size=limit?min(offset+limit, (size_t)finfo.st_size)-offset:finfo.st_size;
		if(!max_size) { // eof
			if(as_text) {
				data=pool.malloc(1);
				*(char*)data=0;
			} else 
				data=0;
			data_size=0;
		} else {
			data=pool.malloc(max_size+(as_text?1:0), 3);
			if(offset)
				lseek(f, offset, SEEK_SET);
			data_size=read(f, data, max_size);
		}
		unlock(f);
		close(f);
		if(!max_size) // eof
			return true;

		if(int(data_size)<0 || data_size>max_size)
			throw Exception(0,
				&file_spec, 
				"read failed: actually read %d bytes count not in [0..%lu] valid range", 
					data_size, (unsigned long)max_size); //never

		if(as_text) {
			fix_line_breaks((char *)data, data_size);
			// note: after fixing
			((char*&)data)[data_size]=0;
		}
		return true;
    } else {
		if(fail_on_read_problem)
			throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0,
				&file_spec, 
				"read failed: %s (%d), actual filename '%s'", 
					strerror(errno), errno, fname);
		return false;
	}
}

static void create_dir_for_file(const String& file_spec) {
	size_t pos_after=1;
	int pos_before;
	while((pos_before=file_spec.pos("/", 1, pos_after))>=0) {
		mkdir(file_spec.mid(0, pos_before).cstr(String::UL_FILE_SPEC), 0775);
		pos_after=pos_before+1;
	}
}

bool file_write_action_under_lock(
				const String& file_spec, 
				const char *action_name, void (*action)(int, void *), void *context,
				bool as_text,
				bool do_append,
				bool do_block,
				bool fail_on_lock_problem) {
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	int f;
	if(access(fname, W_OK)!=0) // no
		create_dir_for_file(file_spec);

	if((f=open(fname, 
		O_CREAT|O_RDWR
		|(as_text?_O_TEXT:_O_BINARY)
		|(do_append?O_APPEND:O_TRUNC), 0664))>=0) {
		if((do_block?lock_exclusive_blocking(f):lock_exclusive_nonblocking(f))!=0) {
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
#if O_TRUNC==0
			if(!do_append)
				ftruncate(f, tell(f));
#endif
			unlock(f);
			close(f);
			/*re*/throw;
		}
		
#if O_TRUNC==0
		if(!do_append)
			ftruncate(f, tell(f));
#endif
		unlock(f);
		close(f);
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
	const void *data; size_t size;
};
#endif
static void file_write_action(int f, void *context) {
	File_write_action_info& info=*static_cast<File_write_action_info *>(context);
	if(info.size) {
		int written=write(f, info.data, info.size);
		if(written<0)
			throw Exception(0,
				0,
				"write failed: %s (%d)",  strerror(errno), errno);
	}
}
void file_write(
				const String& file_spec, 
				const void *data, size_t size, 
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
	int pos_before;
	if((pos_before=file_spec.pos("/", 1, pos_after))>=0)
		rmdir(file_spec, pos_before+1);
	
	rmdir(file_spec.mid(0, pos_after-1/* / */).cstr(String::UL_FILE_SPEC));
}
bool file_delete(const String& file_spec, bool fail_on_read_problem) {
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	if(unlink(fname)!=0)
		if(fail_on_read_problem)
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
	const char *old_spec_cstr=old_spec.cstr(String::UL_FILE_SPEC);
	const char *new_spec_cstr=new_spec.cstr(String::UL_FILE_SPEC);
	
	create_dir_for_file(new_spec);

	if(rename(old_spec_cstr, new_spec_cstr)!=0)
		throw Exception(errno==EACCES?"file.access":errno==ENOENT?"file.missing":0,
			&old_spec, 
			"rename failed: %s (%d), actual filename '%s' to '%s'", 
				strerror(errno), errno, old_spec_cstr, new_spec_cstr);

	rmdir(old_spec, 1);
}


bool entry_exists(const char *fname, struct stat *afinfo) {
	struct stat lfinfo;
	bool result=stat(fname, &lfinfo)==0;
	if(afinfo)
		*afinfo=lfinfo;
	return result;
}

bool entry_exists(const String& file_spec) {
	const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	return entry_exists(fname, 0);
}

static bool entry_readable(const String& file_spec, bool need_dir) {
    const char *fname=file_spec.cstr(String::UL_FILE_SPEC);
	struct stat finfo;
	if(access(fname, R_OK)==0 && entry_exists(fname, &finfo)) {
		bool is_dir=(finfo.st_mode&S_IFDIR) != 0;
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
	
	char *pool_buf=(char *)pool.malloc(size+1, 4);
	memcpy(pool_buf, local_buf, size+1);
	return pool_buf;
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


/// must be last in this file
#undef vsnprintf
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

int pa_sleep(unsigned long secs, unsigned long usecs) {
	for (; usecs >= 1000000; ++secs, usecs -= 1000000);

#ifdef WIN32
	Sleep(secs * 1000 + usecs / 1000);
	return 0;
#else
	struct timeval t;
	t.tv_sec = secs;
	t.tv_usec = usecs;
	return (select(0, NULL, NULL, NULL, &t) == -1 ? errno : 0);
#endif
}
