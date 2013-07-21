/* ====================================================================
 * The Apache Software License, Version 1.1
 *
 * Copyright (c) 2000-2002 The Apache Software Foundation.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        Apache Software Foundation (http://www.apache.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "Apache" and "Apache Software Foundation" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache",
 *    nor may "Apache" appear in their name, without prior written
 *    permission of the Apache Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE APACHE SOFTWARE FOUNDATION OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 */

#ifndef PA_FILE_IO_H
#define PA_FILE_IO_H
/**
 * @file pa_file_io.h
 * @brief APR File I/O Handling
 */
/**
 * @defgroup PA_File_IO_Handle I/O Handling Functions
 * @ingroup PA_File_Handle
 * @{
 */

#include "pa_apr.h"
#include "pa_errno.h"
#include "pa_file_info.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup pa_file_open File Open Flags/Routines
 * @{
 */

#define PA_READ       1           /**< Open the file for reading */
#define PA_WRITE      2           /**< Open the file for writing */
#define PA_CREATE     4           /**< Create the file if not there */
#define PA_APPEND     8           /**< Append to the end of the file */
#define PA_TRUNCATE   16          /**< Open the file and truncate to 0 length */
#define PA_BINARY     32          /**< Open the file in binary mode */
#define PA_EXCL       64          /**< Open should fail if PA_CREATE and file
                                        exists. */
#define PA_BUFFERED   128         /**< Open the file for buffered I/O */
#define PA_DELONCLOSE 256         /**< Delete the file after close */
#define PA_XTHREAD    512         /**< Platform dependent tag to open the file
                                        for use across multiple threads */
#define PA_SHARELOCK  1024        /**< Platform dependent support for higher
                                        level locked read/write access to support
                                        writes across process/machines */
#define PA_FILE_NOCLEANUP  2048   /**< Do not register a cleanup when the file
                                        is opened */
 
/** @} */

/**
 * @defgroup PA_file_seek_flags File Seek Flags
 * @{
 */

/* flags for pa_file_seek */
#define PA_SET SEEK_SET
#define PA_CUR SEEK_CUR
#define PA_END SEEK_END
/** @} */

/** should be same as whence type in lseek, POSIX defines this as int */
typedef int       pa_seek_where_t;

/**
 * Structure for referencing files.
 * @defvar pa_file_t
 */
typedef struct pa_file_t         pa_file_t;

/* File lock types/flags */
/**
 * @defgroup PA_file_lock_types File Lock Types
 * @{
 */

#define PA_FLOCK_SHARED        1       /**< Shared lock. More than one process
                                           or thread can hold a shared lock
                                           at any given time. Essentially,
                                           this is a "read lock", preventing
                                           writers from establishing an
                                           exclusive lock. */
#define PA_FLOCK_EXCLUSIVE     2       /**< Exclusive lock. Only one process
                                           may hold an exclusive lock at any
                                           given time. This is analogous to
                                           a "write lock". */

#define PA_FLOCK_TYPEMASK      0x000F  /**< mask to extract lock type */
#define PA_FLOCK_NONBLOCK      0x0010  /**< do not block while acquiring the
                                           file lock */
/** @} */

/**
 * Open the specified file.
 * @param new_file The opened file descriptor.
 * @param fname The full path to the file (using / on all systems)
 * @param flag Or'ed value of:
 * <PRE>
 *           PA_READ             open for reading
 *           PA_WRITE            open for writing
 *           PA_CREATE           create the file if not there
 *           PA_APPEND           file ptr is set to end prior to all writes
 *           PA_TRUNCATE         set length to zero if file exists
 *           PA_BINARY           not a text file (This flag is ignored on 
 *                                UNIX because it has no meaning)
 *           PA_BUFFERED         buffer the data.  Default is non-buffered
 *           PA_EXCL             return error if PA_CREATE and file exists
 *           PA_DELONCLOSE       delete the file after closing.
 *           PA_XTHREAD          Platform dependent tag to open the file
 *                                for use across multiple threads
 *           PA_SHARELOCK        Platform dependent support for higher
 *                                level locked read/write access to support
 *                                writes across process/machines
 *           PA_FILE_NOCLEANUP   Do not register a cleanup with the pool 
 *                                passed in on the <EM>cont</EM> argument (see below).
 *                                The pa_os_file_t handle in pa_file_t will not
 &                                be closed when the pool is destroyed.
 * </PRE>
 * @param perm Access permissions for file.
 * @param cont The pool to use.
 * @ingroup pa_file_open
 * @remark If perm is PA_OS_DEFAULT and the file is being created, appropriate 
 *      default permissions will be used.  *arg1 must point to a valid file_t, 
 *      or NULL (in which case it will be allocated)
 */
pa_status_t pa_file_open(pa_file_t **new_file, const char *fname,
                                   pa_int32_t flag, pa_fileperms_t perm,
                                   pa_pool_t *cont);

/**
 * Close the specified file.
 * @param file The file descriptor to close.
 */
pa_status_t pa_file_close(pa_file_t *file);


/** file (un)locking functions. */

/**
 * Establish a lock on the specified, open file. The lock may be advisory
 * or mandatory, at the discretion of the platform. The lock applies to
 * the file as a whole, rather than a specific range. Locks are established
 * on a per-thread/process basis; a second lock by the same thread will not
 * block.
 * @param thefile The file to lock.
 * @param type The type of lock to establish on the file.
 */
pa_status_t pa_file_lock(pa_file_t *thefile, int type);

/**
 * Remove any outstanding locks on the file.
 * @param thefile The file to unlock.
 */
pa_status_t pa_file_unlock(pa_file_t *thefile);


/**
 * get the specified file's stats.
 * @param finfo Where to store the information about the file.
 * @param wanted The desired pa_finfo_t fields, as a bit flag of PA_FINFO_ values 
 * @param thefile The file to get information about.
 */ 
pa_status_t pa_file_info_get(pa_finfo_t *finfo, 
                                          pa_int32_t wanted,
                                          pa_file_t *thefile);

/**
 * Move the read/write file offset to a specified byte within a file.
 * @param thefile The file descriptor
 * @param where How to move the pointer, one of:
 * <PRE>
 *            PA_SET  --  set the offset to offset
 *            PA_CUR  --  add the offset to the current position 
 *            PA_END  --  add the offset to the current file size 
 * </PRE>
 * @param offset The offset to move the pointer to.
 * @remark The third argument is modified to be the offset the pointer
          was actually moved to.
 */
pa_status_t pa_file_seek(pa_file_t *thefile, 
                                   pa_seek_where_t where,
                                   pa_off_t *offset);


/**
 * Read data from the specified file, ensuring that the buffer is filled
 * before returning.
 * @param thefile The file descriptor to read from.
 * @param buf The buffer to store the data to.
 * @param nbytes The number of bytes to read.
 * @param bytes_read If non-NULL, this will contain the number of bytes read.
 * @remark pa_file_read will read up to the specified number of bytes, but never 
 *      more.  If there isn't enough data to fill that number of bytes, 
 *      then the process/thread will block until it is available or EOF 
 *      is reached.  If a char was put back into the stream via ungetc, 
 *      it will be the first character returned. 
 *
 *      It is possible for both bytes to be read and an error to be 
 *      returned.  And if *bytes_read is less than nbytes, an
 *      accompanying error is _always_ returned.
 *
 *      PA_EINTR is never returned.
 */
pa_status_t pa_file_read_full(pa_file_t *thefile, void *buf,
                                        pa_size_t nbytes,
                                        pa_size_t *bytes_read);

/**
 * Write data to the specified file, ensuring that all of the data is
 * written before returning.
 * @param thefile The file descriptor to write to.
 * @param buf The buffer which contains the data.
 * @param nbytes The number of bytes to write.
 * @param bytes_written If non-NULL, this will contain the number of bytes written.
 * @remark pa_file_write will write up to the specified number of bytes, but never 
 *      more.  If the OS cannot write that many bytes, the process/thread 
 *      will block until they can be written. Exceptional error such as 
 *      "out of space" or "pipe closed" will terminate with an error.
 *
 *      It is possible for both bytes to be written and an error to be 
 *      returned.  And if *bytes_written is less than nbytes, an
 *      accompanying error is _always_ returned.
 *
 *      PA_EINTR is never returned.
 */
pa_status_t pa_file_write_full(pa_file_t *thefile, const void *buf,
                                         pa_size_t nbytes, 
                                         pa_size_t *bytes_written);

#ifdef __cplusplus
}
#endif
/** @} */
#endif  /* ! PA_FILE_IO_H */
