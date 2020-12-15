/** @file
	Parser: implementation of apr functions.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_file_io.h"
#include "pa_memory.h"
#include "pa_os.h"

volatile const char * IDENT_PA_FILE_IO_C="$Id: pa_file_io.C,v 1.6 2020/12/15 17:10:33 moko Exp $";

struct pa_file_t {
    int handle;
};

pa_status_t pa_file_open(pa_file_t **new_file, const char *fname,
                                   pa_int32_t flag, pa_fileperms_t perm,
								   pa_pool_t *)
{
    int oflags = 0;
#if PA_HAS_THREADS
    pa_status_t rv;
#endif

    (*new_file) = (pa_file_t*)pa_malloc_atomic(sizeof(pa_file_t));
//    (*new_file)->flags = flag;
    (*new_file)->handle = -1;

    if ((flag & PA_READ) && (flag & PA_WRITE)) {
        oflags = O_RDWR;
    }
    else if (flag & PA_READ) {
        oflags = O_RDONLY;
    }
    else if (flag & PA_WRITE) {
        oflags = O_WRONLY;
    }
    else {
        return PA_EACCES; 
    }

	if (flag & PA_CREATE) {
        oflags |= O_CREAT; 
        if (flag & PA_EXCL) {
            oflags |= O_EXCL;
        }
    }
    if ((flag & PA_EXCL) && !(flag & PA_CREATE)) {
        return PA_EACCES;
    }   

    if (flag & PA_APPEND) {
        oflags |= O_APPEND;
    }
    if (flag & PA_TRUNCATE) {
        oflags |= O_TRUNC;
    }
#ifdef O_BINARY
    if (flag & PA_BINARY) {
        oflags |= O_BINARY;
    }
#endif
    
	if(((*new_file)->handle = open(fname, oflags, /*pa_unix_perms2mode*/(perm))) <0 ) 
		return errno;

    return PA_SUCCESS;
}

pa_status_t pa_file_close(pa_file_t *file)
{
	return close(file->handle);
}

pa_status_t pa_file_lock(pa_file_t *file, int type)
{
	if(type & PA_FLOCK_NONBLOCK)
		pa_lock_exclusive_blocking(file->handle);

	if ((type & PA_FLOCK_TYPEMASK) == PA_FLOCK_SHARED)
		return pa_lock_shared_blocking(file->handle);

	return pa_lock_exclusive_blocking(file->handle);
}

pa_status_t pa_file_unlock(pa_file_t *file)
{
	return pa_unlock(file->handle);
}

pa_status_t pa_file_info_get(pa_finfo_t *finfo, 
                                          pa_int32_t /*wanted*/,
                                          pa_file_t *file)
{
    struct stat info;

    if (fstat(file->handle, &info) == 0) {
		finfo->size=info.st_size;
        return PA_SUCCESS;
    }
    else {
        return errno;
    }
}


pa_status_t pa_file_seek(pa_file_t *file, 
                                   pa_seek_where_t where,
                                   pa_off_t *offset)
{
    pa_off_t rv = lseek(file->handle, *offset, where);
    *offset = rv;
	return rv == -1? errno: PA_SUCCESS;
}


pa_status_t pa_file_read_full(pa_file_t *file, void *buf,
                                        pa_size_t nbytes,
                                        pa_size_t *p_bytes_read)
{
	int l_bytes_read = read(file->handle, buf, nbytes);
    if (l_bytes_read == 0) {
        return PA_EOF;
    } else if (l_bytes_read == -1) {
        return errno;
    }

	if(p_bytes_read)
		*p_bytes_read=(pa_size_t)l_bytes_read;

	return PA_SUCCESS;
}


pa_status_t pa_file_write_full(pa_file_t *file, const void *buf,
                                         pa_size_t nbytes, 
                                         pa_size_t *bytes_written)
{
    pa_size_t rv;
    do {
        rv = write(file->handle, buf, nbytes);
    } while (rv == (pa_size_t)-1 && errno == EINTR);

    if (rv == (pa_size_t)-1) {
		if(bytes_written)
			*bytes_written = 0;
        return errno;
    }
	if(bytes_written)
		*bytes_written=rv;
	return PA_SUCCESS;
}
