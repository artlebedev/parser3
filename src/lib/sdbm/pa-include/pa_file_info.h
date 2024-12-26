/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PA_FILE_INFO_H
#define PA_FILE_INFO_H

#include "pa_apr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @file pa_file_info.h
 * @brief APR File handling
 */

/**
 * Structure for determining file permissions.
 * @defvar pa_fileperms_t
 */
typedef pa_int32_t               pa_fileperms_t;

/**
 * @defgroup PA_File_Info Stat Functions
 * @{
 */
#ifdef LATER
#define PA_FINFO_LINK   0x00000001 /**< Stat the link not the file itself if it is a link */
#define PA_FINFO_MTIME  0x00000010 /**< Modification Time */
#define PA_FINFO_CTIME  0x00000020 /**< Creation Time */
#define PA_FINFO_ATIME  0x00000040 /**< Access Time */
#endif
#define PA_FINFO_SIZE   0x00000100 /**< Size of the file */
#ifdef LATER
#define PA_FINFO_CSIZE  0x00000200 /**< Storage size consumed by the file */
#define PA_FINFO_DEV    0x00001000
#define PA_FINFO_INODE  0x00002000
#define PA_FINFO_NLINK  0x00004000
#define PA_FINFO_TYPE   0x00008000
#define PA_FINFO_USER   0x00010000 
#define PA_FINFO_GROUP  0x00020000 
#define PA_FINFO_UPROT  0x00100000 
#define PA_FINFO_GPROT  0x00200000
#define PA_FINFO_WPROT  0x00400000
#define PA_FINFO_ICASE  0x01000000  /**<  if dev is case insensitive */
#define PA_FINFO_NAME   0x02000000  /**<  ->name in proper case */

#define PA_FINFO_MIN    0x00008170  /**<  type, mtime, ctime, atime, size */
#define PA_FINFO_IDENT  0x00003000  /**<  dev and inode */
#define PA_FINFO_OWNER  0x00030000  /**<  user and group */
#define PA_FINFO_PROT   0x00700000  /**<  all protections */
#define PA_FINFO_NORM   0x0073b170  /**<  an atomic unix pa_stat() */
#define PA_FINFO_DIRENT 0x02000000  /**<  an atomic unix pa_dir_read() */
#endif

/**
 * The file information structure.  This is analogous to the POSIX
 * stat structure.
 */
typedef struct pa_finfo_t        pa_finfo_t;

struct pa_finfo_t {
#ifdef LATER
	/** Allocates memory and closes lingering handles in the specified pool */
    pa_pool_t *pool;
    /** The bitmask describing valid fields of this pa_finfo_t structure 
     *  including all available 'wanted' fields and potentially more */
    pa_int32_t valid;
    /** The access permissions of the file.  Mimics Unix access rights. */
    pa_fileperms_t protection;
    /** The type of file.  One of PA_NOFILE, PA_REG, PA_DIR, PA_CHR, 
     *  PA_BLK, PA_PIPE, PA_LNK, PA_SOCK 
     */
    pa_filetype_e filetype;
    /** The user id that owns the file */
    pa_uid_t user;
    /** The group id that owns the file */
    pa_gid_t group;
    /** The inode of the file. */
    pa_ino_t inode;
    /** The id of the device the file is on. */
    pa_dev_t device;
    /** The number of hard links to the file. */
    pa_int32_t nlink;
#endif
    /** The size of the file */
    pa_off_t size;
#ifdef LATER
    /** The storage size consumed by the file */
    pa_off_t csize;
    /** The time the file was last accessed */
    pa_time_t atime;
    /** The time the file was last modified */
    pa_time_t mtime;
    /** The time the file was last changed */
    pa_time_t ctime;
    /** The full pathname of the file */
    const char *fname;
    /** The file's name (no path) in filesystem case */
    const char *name;
    /** The file's handle, if accessed (can be submitted to pa_duphandle) */
    struct pa_file_t *filehand;
#endif
};

/** @} */



#ifdef __cplusplus
}
#endif

#endif  /* ! PA_FILE_INFO_H */
