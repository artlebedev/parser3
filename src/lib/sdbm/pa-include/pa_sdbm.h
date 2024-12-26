/* Copyright 2000-2005 The Apache Software Foundation or its licensors, as
 * applicable.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * sdbm - ndbm work-alike hashed database library
 * based on Per-Ake Larson's Dynamic Hashing algorithms. BIT 18 (1978).
 * author: oz@nexus.yorku.ca
 * status: ex-public domain
 */

#ifndef PA_SDBM_H
#define PA_SDBM_H

#include "pa_apr.h"
#include "pa_errno.h"
#include "pa_file_io.h"   /* for pa_fileperms_t */

#ifdef __cplusplus
extern "C" {
#endif


/** 
 * @file pa_sdbm.h
 * @brief apr-util SDBM library
 */
/**
 * @defgroup PA_Util_DBM_SDBM SDBM library
 * @ingroup PA_Util_DBM
 * @{
 */

/**
 * Structure for referencing an sdbm
 */
typedef struct pa_sdbm_t pa_sdbm_t;

/**
 * Structure for referencing the datum record within an sdbm
 */
typedef struct {
    /** pointer to the data stored/retrieved */
    char *dptr;
    /** size of data */
    int dsize;
} pa_sdbm_datum_t;

/* The extensions used for the database files */
#define PA_SDBM_DIRFEXT	".dir"
#define PA_SDBM_PAGFEXT	".pag"

/* flags to sdbm_store */
#define PA_SDBM_INSERT     0
#define PA_SDBM_REPLACE    1
#define PA_SDBM_INSERTDUP  2

/**
 * Open an sdbm database by file name
 * @param db The newly opened database
 * @param name The sdbm file to open
 * @param mode The flag values (PA_READ and PA_BINARY flags are implicit)
 * <PRE>
 *           PA_WRITE          open for read-write access
 *           PA_CREATE         create the sdbm if it does not exist
 *           PA_TRUNCATE       empty the contents of the sdbm
 *           PA_EXCL           fail for PA_CREATE if the file exists
 *           PA_SHARELOCK      support locking across process/machines
 * </PRE>
 * @param perm Permissions to apply to if created
 * @param pool The pool to use when creating the sdbm
 * @remark The sdbm name is not a true file name, as sdbm appends suffixes 
 * for seperate data and index files.
 */
pa_status_t pa_sdbm_open(pa_sdbm_t **db, const char *name, 
                                        pa_int32_t mode, 
                                        pa_fileperms_t perms, pa_pool_t *p);

/**
 * Close an sdbm file previously opened by pa_sdbm_open
 * @param db The database to close
 */
pa_status_t pa_sdbm_close(pa_sdbm_t *db);

/**
 * Lock an sdbm database for concurency of multiple operations
 * @param db The database to lock
 * @param type The lock type
 * <PRE>
 *           PA_FLOCK_SHARED
 *           PA_FLOCK_EXCLUSIVE
 * </PRE>
 * @remark Calls to pa_sdbm_lock may be nested.  All pa_sdbm functions
 * perform implicit locking.  Since an PA_FLOCK_SHARED lock cannot be 
 * portably promoted to an PA_FLOCK_EXCLUSIVE lock, pa_sdbm_store and 
 * pa_sdbm_delete calls will fail if an PA_FLOCK_SHARED lock is held.
 * The pa_sdbm_lock call requires the database to be opened with the
 * PA_SHARELOCK mode value.
 */
pa_status_t pa_sdbm_lock(pa_sdbm_t *db, int type);

/**
 * Release an sdbm lock previously aquired by pa_sdbm_lock
 * @param db The database to unlock
 */
pa_status_t pa_sdbm_unlock(pa_sdbm_t *db);

/**
 * Fetch an sdbm record value by key
 * @param db The database 
 * @param value The value datum retrieved for this record
 * @param key The key datum to find this record
 */
pa_status_t pa_sdbm_fetch(pa_sdbm_t *db, 
                                         pa_sdbm_datum_t *value, 
                                         pa_sdbm_datum_t key);

/**
 * Store an sdbm record value by key
 * @param db The database 
 * @param key The key datum to store this record by
 * @param value The value datum to store in this record
 * @param opt The method used to store the record
 * <PRE>
 *           PA_SDBM_INSERT     return an error if the record exists
 *           PA_SDBM_REPLACE    overwrite any existing record for key
 * </PRE>
 */
pa_status_t pa_sdbm_store(pa_sdbm_t *db, pa_sdbm_datum_t key,
                                         pa_sdbm_datum_t value, int opt);

/**
 * Delete an sdbm record value by key
 * @param db The database 
 * @param key The key datum of the record to delete
 * @remark It is not an error to delete a non-existent record.
 */
pa_status_t pa_sdbm_delete(pa_sdbm_t *db, 
                                          const pa_sdbm_datum_t key);

/**
 * Retrieve the first record key from a dbm
 * @param dbm The database 
 * @param key The key datum of the first record
 * @remark The keys returned are not ordered.  To traverse the list of keys
 * for an sdbm opened with PA_SHARELOCK, the caller must use pa_sdbm_lock
 * prior to retrieving the first record, and hold the lock until after the
 * last call to pa_sdbm_nextkey.
 */
pa_status_t pa_sdbm_firstkey(pa_sdbm_t *db, pa_sdbm_datum_t *key);

/**
 * Retrieve the next record key from an sdbm
 * @param db The database 
 * @param key The key datum of the next record
 */
pa_status_t pa_sdbm_nextkey(pa_sdbm_t *db, pa_sdbm_datum_t *key);

/**
 * Returns true if the sdbm database opened for read-only access
 * @param db The database to test
 */
int pa_sdbm_rdonly(pa_sdbm_t *db);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PA_SDBM_H */
