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

#ifndef APR_FILE_INFO_H
#define APR_FILE_INFO_H

#include "apr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @file apr_file_info.h
 * @brief APR File handling
 */

/**
 * Structure for determining file permissions.
 * @defvar apr_fileperms_t
 */
typedef apr_int32_t               apr_fileperms_t;

/**
 * @defgroup APR_File_Info Stat Functions
 * @{
 */
#ifdef LATER
#define APR_FINFO_LINK   0x00000001 /**< Stat the link not the file itself if it is a link */
#define APR_FINFO_MTIME  0x00000010 /**< Modification Time */
#define APR_FINFO_CTIME  0x00000020 /**< Creation Time */
#define APR_FINFO_ATIME  0x00000040 /**< Access Time */
#endif
#define APR_FINFO_SIZE   0x00000100 /**< Size of the file */
#ifdef LATER
#define APR_FINFO_CSIZE  0x00000200 /**< Storage size consumed by the file */
#define APR_FINFO_DEV    0x00001000
#define APR_FINFO_INODE  0x00002000
#define APR_FINFO_NLINK  0x00004000
#define APR_FINFO_TYPE   0x00008000
#define APR_FINFO_USER   0x00010000 
#define APR_FINFO_GROUP  0x00020000 
#define APR_FINFO_UPROT  0x00100000 
#define APR_FINFO_GPROT  0x00200000
#define APR_FINFO_WPROT  0x00400000
#define APR_FINFO_ICASE  0x01000000  /**<  if dev is case insensitive */
#define APR_FINFO_NAME   0x02000000  /**<  ->name in proper case */

#define APR_FINFO_MIN    0x00008170  /**<  type, mtime, ctime, atime, size */
#define APR_FINFO_IDENT  0x00003000  /**<  dev and inode */
#define APR_FINFO_OWNER  0x00030000  /**<  user and group */
#define APR_FINFO_PROT   0x00700000  /**<  all protections */
#define APR_FINFO_NORM   0x0073b170  /**<  an atomic unix apr_stat() */
#define APR_FINFO_DIRENT 0x02000000  /**<  an atomic unix apr_dir_read() */
#endif

/**
 * The file information structure.  This is analogous to the POSIX
 * stat structure.
 */
typedef struct apr_finfo_t        apr_finfo_t;

struct apr_finfo_t {
#ifdef LATER
	/** Allocates memory and closes lingering handles in the specified pool */
    apr_pool_t *pool;
    /** The bitmask describing valid fields of this apr_finfo_t structure 
     *  including all available 'wanted' fields and potentially more */
    apr_int32_t valid;
    /** The access permissions of the file.  Mimics Unix access rights. */
    apr_fileperms_t protection;
    /** The type of file.  One of APR_NOFILE, APR_REG, APR_DIR, APR_CHR, 
     *  APR_BLK, APR_PIPE, APR_LNK, APR_SOCK 
     */
    apr_filetype_e filetype;
    /** The user id that owns the file */
    apr_uid_t user;
    /** The group id that owns the file */
    apr_gid_t group;
    /** The inode of the file. */
    apr_ino_t inode;
    /** The id of the device the file is on. */
    apr_dev_t device;
    /** The number of hard links to the file. */
    apr_int32_t nlink;
#endif
    /** The size of the file */
    apr_off_t size;
#ifdef LATER
    /** The storage size consumed by the file */
    apr_off_t csize;
    /** The time the file was last accessed */
    apr_time_t atime;
    /** The time the file was last modified */
    apr_time_t mtime;
    /** The time the file was last changed */
    apr_time_t ctime;
    /** The full pathname of the file */
    const char *fname;
    /** The file's name (no path) in filesystem case */
    const char *name;
    /** The file's handle, if accessed (can be submitted to apr_duphandle) */
    struct apr_file_t *filehand;
#endif
};

/** @} */



#ifdef __cplusplus
}
#endif

#endif  /* ! APR_FILE_INFO_H */
