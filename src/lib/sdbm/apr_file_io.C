/** @file
	Parser: implementation of apr functions.

	Copyright(c) 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2005/11/18 10:04:03 $";

#include "apr_file_io.h"

#include "pa_memory.h"
#include "pa_os.h"

struct apr_file_t {
    int handle;
};

APR_DECLARE(apr_status_t) apr_file_open(apr_file_t **new_file, const char *fname,
                                   apr_int32_t flag, apr_fileperms_t perm,
								   apr_pool_t *)
{
    int oflags = 0;
#if APR_HAS_THREADS
    apr_status_t rv;
#endif

    (*new_file) = (apr_file_t*)pa_malloc_atomic(sizeof(apr_file_t));
//    (*new_file)->flags = flag;
    (*new_file)->handle = -1;

    if ((flag & APR_READ) && (flag & APR_WRITE)) {
        oflags = O_RDWR;
    }
    else if (flag & APR_READ) {
        oflags = O_RDONLY;
    }
    else if (flag & APR_WRITE) {
        oflags = O_WRONLY;
    }
    else {
        return APR_EACCES; 
    }

	if (flag & APR_CREATE) {
        oflags |= O_CREAT; 
        if (flag & APR_EXCL) {
            oflags |= O_EXCL;
        }
    }
    if ((flag & APR_EXCL) && !(flag & APR_CREATE)) {
        return APR_EACCES;
    }   

    if (flag & APR_APPEND) {
        oflags |= O_APPEND;
    }
    if (flag & APR_TRUNCATE) {
        oflags |= O_TRUNC;
    }
#ifdef O_BINARY
    if (flag & APR_BINARY) {
        oflags |= O_BINARY;
    }
#endif
    
	if(((*new_file)->handle = open(fname, oflags, /*apr_unix_perms2mode*/(perm))) <0 ) 
		return errno;

    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) apr_file_close(apr_file_t *file)
{
	return close(file->handle);
}

APR_DECLARE(apr_status_t) apr_file_lock(apr_file_t *file, int type)
{
	if(type & APR_FLOCK_NONBLOCK)
		pa_lock_exclusive_nonblocking(file->handle);

	if ((type & APR_FLOCK_TYPEMASK) == APR_FLOCK_SHARED)
		return pa_lock_shared_blocking(file->handle);

	return pa_lock_exclusive_blocking(file->handle);
}

APR_DECLARE(apr_status_t) apr_file_unlock(apr_file_t *file)
{
	return pa_unlock(file->handle);
}

APR_DECLARE(apr_status_t) apr_file_info_get(apr_finfo_t *finfo, 
                                          apr_int32_t /*wanted*/,
                                          apr_file_t *file)
{
    struct stat info;

    if (fstat(file->handle, &info) == 0) {
		finfo->size=info.st_size;
        return APR_SUCCESS;
    }
    else {
        return errno;
    }
}


APR_DECLARE(apr_status_t) apr_file_seek(apr_file_t *file, 
                                   apr_seek_where_t where,
                                   apr_off_t *offset)
{
    apr_off_t rv = lseek(file->handle, *offset, where);
    *offset = rv;
	return rv == -1? errno: APR_SUCCESS;
}


APR_DECLARE(apr_status_t) apr_file_read_full(apr_file_t *file, void *buf,
                                        apr_size_t nbytes,
                                        apr_size_t *p_bytes_read)
{
	int l_bytes_read = read(file->handle, buf, nbytes);
    if (l_bytes_read == 0)
        return APR_EOF;
    else if (l_bytes_read == -1)
        return errno;

	if(p_bytes_read)
		*p_bytes_read=(apr_size_t)l_bytes_read;

	return APR_SUCCESS;
}


APR_DECLARE(apr_status_t) apr_file_write_full(apr_file_t *file, const void *buf,
                                         apr_size_t nbytes, 
                                         apr_size_t *bytes_written)
{
    apr_size_t rv;
    do {
        rv = write(file->handle, buf, nbytes);
    } while (rv == (apr_size_t)-1 && errno == EINTR);

    if (rv == (apr_size_t)-1) {
		if(bytes_written)
			*bytes_written = 0;
        return errno;
    }
	if(bytes_written)
		*bytes_written=rv;
	return APR_SUCCESS;
}
