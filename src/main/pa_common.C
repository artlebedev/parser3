/*
  $Id: pa_common.C,v 1.2 2001/02/20 18:45:53 paf Exp $
*/

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>


#include "pa_common.h"
#include "pa_types.h"

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

char *file_read(Pool& pool, char *fname) {
    int f;
    struct stat finfo;
    if (fname && !stat(fname,&finfo) && (f=open(fname,O_RDONLY
#ifdef WIN32
		|O_TEXT
#endif
		))>=0){
		/*if(exclusive)
			flock(f, LOCK_EX);*/

		char *result=static_cast<char *>(pool.malloc(finfo.st_size+1));
		int read_size=read(f,result,finfo.st_size);
		if(read_size>=0 && read_size<=finfo.st_size) 
			result[read_size]='\0';
		/*if(exclusive)
			flock(f, LOCK_UN);*/
		close(f);
		return result;//prepare_config(result, remove_empty_lines);
    }
	// maybe: exception?
    return NULL;
}
