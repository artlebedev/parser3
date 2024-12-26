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

#ifndef PA_ERRNO_H
#define PA_ERRNO_H

#include "pa_apr.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file pa_errno.h
 * @brief APR Error Codes
 */
/**
 * @defgroup PA_Error_Codes Error Codes
 * @ingroup APR
 * @{
 */

/**
 * Type for specifying an error or status code.
 */
typedef int pa_status_t;

/**
 * PA_OS_START_ERROR is where the APR specific error values start.
 */
#define PA_OS_START_ERROR     20000
/**
 * PA_OS_ERRSPACE_SIZE is the maximum number of errors you can fit
 *    into one of the error/status ranges below -- except for
 *    PA_OS_START_USERERR, which see.
 */
#define PA_OS_ERRSPACE_SIZE 50000
/**
 * PA_OS_START_STATUS is where the APR specific status codes start.
 */
#define PA_OS_START_STATUS    (PA_OS_START_ERROR + PA_OS_ERRSPACE_SIZE)
/**
 * PA_OS_START_USERERR are reserved for applications that use APR that
 *     layer their own error codes along with APR's.  Note that the
 *     error immediately following this one is set ten times farther
 *     away than usual, so that users of apr have a lot of room in
 *     which to declare custom error codes.
 */
#define PA_OS_START_USERERR    (PA_OS_START_STATUS + PA_OS_ERRSPACE_SIZE)
/**
 * PA_OS_START_CANONERR is where APR versions of errno values are defined
 *     on systems which don't have the corresponding errno.
 */
#define PA_OS_START_CANONERR  (PA_OS_START_USERERR \
                                 + (PA_OS_ERRSPACE_SIZE * 10))

/** no error. @see PA_STATUS_IS_SUCCESS */
#define PA_SUCCESS 0

/* APR ERROR VALUES */
/** 
 * @defgroup APRErrorValues Error Values
 * <PRE>
 * <b>APR ERROR VALUES</b>
 * </PRE>
 *
 * <PRE>
 * <b>APR STATUS VALUES</b>
 * PA_EOF            APR has encountered the end of the file
 * </PRE>
 * @{
 */
#define PA_EGENERAL       (PA_OS_START_ERROR + 14)
#define PA_EOF            (PA_OS_START_STATUS + 14)

/**
 * @defgroup aprerrcanonical Canonical Errors
 * @{
 */
/* APR CANONICAL ERROR VALUES */
/** @see PA_STATUS_IS_EACCES */
#ifdef EACCES
#define PA_EACCES EACCES
#else
#define PA_EACCES         (PA_OS_START_CANONERR + 1)
#endif

/** @see PA_STATUS_IS_EXIST */
#ifdef EEXIST
#define PA_EEXIST EEXIST
#else
#define PA_EEXIST         (PA_OS_START_CANONERR + 2)
#endif

/** @see PA_STATUS_IS_ENOSPC */
#ifdef ENOSPC
#define PA_ENOSPC ENOSPC
#else
#define PA_ENOSPC         (PA_OS_START_CANONERR + 6)
#endif

/** @see PA_STATUS_IS_EINVAL */
#ifdef EINVAL
#define PA_EINVAL EINVAL
#else
#define PA_EINVAL         (PA_OS_START_CANONERR + 11)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* ! PA_ERRNO_H */
