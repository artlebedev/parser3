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

#ifndef APR_ERRNO_H
#define APR_ERRNO_H

#include "apr.h"

#if APR_HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @file apr_errno.h
 * @brief APR Error Codes
 */
/**
 * @defgroup APR_Error_Codes Error Codes
 * @ingroup APR
 * @{
 */

/**
 * Type for specifying an error or status code.
 */
typedef int apr_status_t;

/**
 * APR_OS_START_ERROR is where the APR specific error values start.
 */
#define APR_OS_START_ERROR     20000
/**
 * APR_OS_ERRSPACE_SIZE is the maximum number of errors you can fit
 *    into one of the error/status ranges below -- except for
 *    APR_OS_START_USERERR, which see.
 */
#define APR_OS_ERRSPACE_SIZE 50000
/**
 * APR_OS_START_STATUS is where the APR specific status codes start.
 */
#define APR_OS_START_STATUS    (APR_OS_START_ERROR + APR_OS_ERRSPACE_SIZE)
/**
 * APR_OS_START_USERERR are reserved for applications that use APR that
 *     layer their own error codes along with APR's.  Note that the
 *     error immediately following this one is set ten times farther
 *     away than usual, so that users of apr have a lot of room in
 *     which to declare custom error codes.
 */
#define APR_OS_START_USERERR    (APR_OS_START_STATUS + APR_OS_ERRSPACE_SIZE)
/**
 * APR_OS_START_USEERR is obsolete, defined for compatibility only.
 * Use APR_OS_START_USERERR instead.
 */
#define APR_OS_START_USEERR     APR_OS_START_USERERR
/**
 * APR_OS_START_CANONERR is where APR versions of errno values are defined
 *     on systems which don't have the corresponding errno.
 */
#define APR_OS_START_CANONERR  (APR_OS_START_USERERR \
                                 + (APR_OS_ERRSPACE_SIZE * 10))
/**
 * APR_OS_START_EAIERR folds EAI_ error codes from getaddrinfo() into 
 *     apr_status_t values.
 */
#define APR_OS_START_EAIERR    (APR_OS_START_CANONERR + APR_OS_ERRSPACE_SIZE)
/**
 * APR_OS_START_SYSERR folds platform-specific system error values into 
 *     apr_status_t values.
 */
#define APR_OS_START_SYSERR    (APR_OS_START_EAIERR + APR_OS_ERRSPACE_SIZE)

/** no error. @see APR_STATUS_IS_SUCCESS */
#define APR_SUCCESS 0

/* APR ERROR VALUES */
/** 
 * @defgroup APRErrorValues Error Values
 * <PRE>
 * <b>APR ERROR VALUES</b>
 * APR_ENOSTAT      APR was unable to perform a stat on the file 
 * APR_ENOPOOL      APR was not provided a pool with which to allocate memory
 * APR_EBADDATE     APR was given an invalid date 
 * APR_EINVALSOCK   APR was given an invalid socket
 * APR_ENOPROC      APR was not given a process structure
 * APR_ENOTIME      APR was not given a time structure
 * APR_ENODIR       APR was not given a directory structure
 * APR_ENOLOCK      APR was not given a lock structure
 * APR_ENOPOLL      APR was not given a poll structure
 * APR_ENOSOCKET    APR was not given a socket
 * APR_ENOTHREAD    APR was not given a thread structure
 * APR_ENOTHDKEY    APR was not given a thread key structure
 * APR_ENOSHMAVAIL  There is no more shared memory available
 * APR_EDSOOPEN     APR was unable to open the dso object.  For more 
 *                  information call apr_dso_error().
 * APR_EGENERAL     General failure (specific information not available)
 * APR_EBADIP       The specified IP address is invalid
 * APR_EBADMASK     The specified netmask is invalid
 * </PRE>
 *
 * <PRE>
 * <b>APR STATUS VALUES</b>
 * APR_INCHILD        Program is currently executing in the child
 * APR_INPARENT       Program is currently executing in the parent
 * APR_DETACH         The thread is detached
 * APR_NOTDETACH      The thread is not detached
 * APR_CHILD_DONE     The child has finished executing
 * APR_CHILD_NOTDONE  The child has not finished executing
 * APR_TIMEUP         The operation did not finish before the timeout
 * APR_INCOMPLETE     The operation was incomplete although some processing
 *                    was performed and the results are partially valid
 * APR_BADCH          Getopt found an option not in the option string
 * APR_BADARG         Getopt found an option that is missing an argument 
 *                    and an argument was specified in the option string
 * APR_EOF            APR has encountered the end of the file
 * APR_NOTFOUND       APR was unable to find the socket in the poll structure
 * APR_ANONYMOUS      APR is using anonymous shared memory
 * APR_FILEBASED      APR is using a file name as the key to the shared memory
 * APR_KEYBASED       APR is using a shared key as the key to the shared memory
 * APR_EINIT          Ininitalizer value.  If no option has been found, but 
 *                    the status variable requires a value, this should be used
 * APR_ENOTIMPL       The APR function has not been implemented on this 
 *                    platform, either because nobody has gotten to it yet, 
 *                    or the function is impossible on this platform.
 * APR_EMISMATCH      Two passwords do not match.
 * APR_EABSOLUTE      The given path was absolute.
 * APR_ERELATIVE      The given path was relative.
 * APR_EINCOMPLETE    The given path was neither relative nor absolute.
 * APR_EABOVEROOT     The given path was above the root path.
 * APR_EBUSY          The given lock was busy.
 * </PRE>
 * @{
 */
/** @see APR_STATUS_IS_ENOSTAT */
#define APR_ENOSTAT        (APR_OS_START_ERROR + 1)
/** @see APR_STATUS_IS_ENOPOOL */
#define APR_ENOPOOL        (APR_OS_START_ERROR + 2)
/* empty slot: +3 */
/** @see APR_STATUS_IS_EBADDATE */
#define APR_EBADDATE       (APR_OS_START_ERROR + 4)
/** @see APR_STATUS_IS_EINVALSOCK */
#define APR_EINVALSOCK     (APR_OS_START_ERROR + 5)
/** @see APR_STATUS_IS_ENOPROC */
#define APR_ENOPROC        (APR_OS_START_ERROR + 6)
/** @see APR_STATUS_IS_ENOTIME */
#define APR_ENOTIME        (APR_OS_START_ERROR + 7)
/** @see APR_STATUS_IS_ENODIR */
#define APR_ENODIR         (APR_OS_START_ERROR + 8)
/** @see APR_STATUS_IS_ENOLOCK */
#define APR_ENOLOCK        (APR_OS_START_ERROR + 9)
/** @see APR_STATUS_IS_ENOPOLL */
#define APR_ENOPOLL        (APR_OS_START_ERROR + 10)
/** @see APR_STATUS_IS_ENOSOCKET */
#define APR_ENOSOCKET      (APR_OS_START_ERROR + 11)
/** @see APR_STATUS_IS_ENOTHREAD */
#define APR_ENOTHREAD      (APR_OS_START_ERROR + 12)
/** @see APR_STATUS_IS_ENOTHDKEY */
#define APR_ENOTHDKEY      (APR_OS_START_ERROR + 13)
/** @see APR_STATUS_IS_EGENERAL */
#define APR_EGENERAL       (APR_OS_START_ERROR + 14)
/** @see APR_STATUS_IS_ENOSHMAVAIL */
#define APR_ENOSHMAVAIL    (APR_OS_START_ERROR + 15)
/** @see APR_STATUS_IS_EBADIP */
#define APR_EBADIP         (APR_OS_START_ERROR + 16)
/** @see APR_STATUS_IS_EBADMASK */
#define APR_EBADMASK       (APR_OS_START_ERROR + 17)
/* empty slot: +18 */
/** @see APR_STATUS_IS_EDSOPEN */
#define APR_EDSOOPEN       (APR_OS_START_ERROR + 19)
/** @see APR_STATUS_IS_EABSOLUTE */
#define APR_EABSOLUTE      (APR_OS_START_ERROR + 20)
/** @see APR_STATUS_IS_ERELATIVE */
#define APR_ERELATIVE      (APR_OS_START_ERROR + 21)
/** @see APR_STATUS_IS_EINCOMPLETE */
#define APR_EINCOMPLETE    (APR_OS_START_ERROR + 22)
/** @see APR_STATUS_IS_EABOVEROOT */
#define APR_EABOVEROOT     (APR_OS_START_ERROR + 23)
/** @see APR_STATUS_IS_EBADPATH */
#define APR_EBADPATH       (APR_OS_START_ERROR + 24)

/* APR STATUS VALUES */
/** @see APR_STATUS_IS_INCHILD */
#define APR_INCHILD        (APR_OS_START_STATUS + 1)
/** @see APR_STATUS_IS_INPARENT */
#define APR_INPARENT       (APR_OS_START_STATUS + 2)
/** @see APR_STATUS_IS_DETACH */
#define APR_DETACH         (APR_OS_START_STATUS + 3)
/** @see APR_STATUS_IS_NOTDETACH */
#define APR_NOTDETACH      (APR_OS_START_STATUS + 4)
/** @see APR_STATUS_IS_CHILD_DONE */
#define APR_CHILD_DONE     (APR_OS_START_STATUS + 5)
/** @see APR_STATUS_IS_CHILD_NOTDONE */
#define APR_CHILD_NOTDONE  (APR_OS_START_STATUS + 6)
/** @see APR_STATUS_IS_TIMEUP */
#define APR_TIMEUP         (APR_OS_START_STATUS + 7)
/** @see APR_STATUS_IS_INCOMPLETE */
#define APR_INCOMPLETE     (APR_OS_START_STATUS + 8)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/** @see APR_STATUS_IS_BADCH */
#define APR_BADCH          (APR_OS_START_STATUS + 12)
/** @see APR_STATUS_IS_BADARG */
#define APR_BADARG         (APR_OS_START_STATUS + 13)
/** @see APR_STATUS_IS_EOF */
#define APR_EOF            (APR_OS_START_STATUS + 14)
/** @see APR_STATUS_IS_NOTFOUND */
#define APR_NOTFOUND       (APR_OS_START_STATUS + 15)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/** @see APR_STATUS_IS_ANONYMOUS */
#define APR_ANONYMOUS      (APR_OS_START_STATUS + 19)
/** @see APR_STATUS_IS_FILEBASED */
#define APR_FILEBASED      (APR_OS_START_STATUS + 20)
/** @see APR_STATUS_IS_KEYBASED */
#define APR_KEYBASED       (APR_OS_START_STATUS + 21)
/** @see APR_STATUS_IS_EINIT */
#define APR_EINIT          (APR_OS_START_STATUS + 22)  
/** @see APR_STATUS_IS_ENOTIMPL */
#define APR_ENOTIMPL       (APR_OS_START_STATUS + 23)
/** @see APR_STATUS_IS_EMISMATCH */
#define APR_EMISMATCH      (APR_OS_START_STATUS + 24)
/** @see APR_STATUS_IS_EBUSY */
#define APR_EBUSY          (APR_OS_START_STATUS + 25)

/**
 * @defgroup aprerrcanonical Canonical Errors
 * @{
 */
/* APR CANONICAL ERROR VALUES */
/** @see APR_STATUS_IS_EACCES */
#ifdef EACCES
#define APR_EACCES EACCES
#else
#define APR_EACCES         (APR_OS_START_CANONERR + 1)
#endif

/** @see APR_STATUS_IS_EXIST */
#ifdef EEXIST
#define APR_EEXIST EEXIST
#else
#define APR_EEXIST         (APR_OS_START_CANONERR + 2)
#endif

/** @see APR_STATUS_IS_ENAMETOOLONG */
#ifdef ENAMETOOLONG
#define APR_ENAMETOOLONG ENAMETOOLONG
#else
#define APR_ENAMETOOLONG   (APR_OS_START_CANONERR + 3)
#endif

/** @see APR_STATUS_IS_ENOENT */
#ifdef ENOENT
#define APR_ENOENT ENOENT
#else
#define APR_ENOENT         (APR_OS_START_CANONERR + 4)
#endif

/** @see APR_STATUS_IS_ENOTDIR */
#ifdef ENOTDIR
#define APR_ENOTDIR ENOTDIR
#else
#define APR_ENOTDIR        (APR_OS_START_CANONERR + 5)
#endif

/** @see APR_STATUS_IS_ENOSPC */
#ifdef ENOSPC
#define APR_ENOSPC ENOSPC
#else
#define APR_ENOSPC         (APR_OS_START_CANONERR + 6)
#endif

/** @see APR_STATUS_IS_ENOMEM */
#ifdef ENOMEM
#define APR_ENOMEM ENOMEM
#else
#define APR_ENOMEM         (APR_OS_START_CANONERR + 7)
#endif

/** @see APR_STATUS_IS_EMFILE */
#ifdef EMFILE
#define APR_EMFILE EMFILE
#else
#define APR_EMFILE         (APR_OS_START_CANONERR + 8)
#endif

/** @see APR_STATUS_IS_ENFILE */
#ifdef ENFILE
#define APR_ENFILE ENFILE
#else
#define APR_ENFILE         (APR_OS_START_CANONERR + 9)
#endif

/** @see APR_STATUS_IS_EBADF */
#ifdef EBADF
#define APR_EBADF EBADF
#else
#define APR_EBADF          (APR_OS_START_CANONERR + 10)
#endif

/** @see APR_STATUS_IS_EINVAL */
#ifdef EINVAL
#define APR_EINVAL EINVAL
#else
#define APR_EINVAL         (APR_OS_START_CANONERR + 11)
#endif

/** @see APR_STATUS_IS_ESPIPE */
#ifdef ESPIPE
#define APR_ESPIPE ESPIPE
#else
#define APR_ESPIPE         (APR_OS_START_CANONERR + 12)
#endif

/** 
 * @see APR_STATUS_IS_EAGAIN 
 * @warning use APR_STATUS_IS_EAGAIN instead of just testing this value
 */
#ifdef EAGAIN
#define APR_EAGAIN EAGAIN
#elif defined(EWOULDBLOCK)
#define APR_EAGAIN EWOULDBLOCK
#else
#define APR_EAGAIN         (APR_OS_START_CANONERR + 13)
#endif

/** @see APR_STATUS_IS_EINTR */
#ifdef EINTR
#define APR_EINTR EINTR
#else
#define APR_EINTR          (APR_OS_START_CANONERR + 14)
#endif

/** @see APR_STATUS_IS_ENOTSOCK */
#ifdef ENOTSOCK
#define APR_ENOTSOCK ENOTSOCK
#else
#define APR_ENOTSOCK       (APR_OS_START_CANONERR + 15)
#endif

/** @see APR_STATUS_IS_ECONNREFUSED */
#ifdef ECONNREFUSED
#define APR_ECONNREFUSED ECONNREFUSED
#else
#define APR_ECONNREFUSED   (APR_OS_START_CANONERR + 16)
#endif

/** @see APR_STATUS_IS_EINPROGRESS */
#ifdef EINPROGRESS
#define APR_EINPROGRESS EINPROGRESS
#else
#define APR_EINPROGRESS    (APR_OS_START_CANONERR + 17)
#endif

/** 
 * @see APR_STATUS_IS_ECONNABORTED
 * @warning use APR_STATUS_IS_ECONNABORTED instead of just testing this value
 */

#ifdef ECONNABORTED
#define APR_ECONNABORTED ECONNABORTED
#else
#define APR_ECONNABORTED   (APR_OS_START_CANONERR + 18)
#endif

/** @see APR_STATUS_IS_ECONNRESET */
#ifdef ECONNRESET
#define APR_ECONNRESET ECONNRESET
#else
#define APR_ECONNRESET     (APR_OS_START_CANONERR + 19)
#endif

/** @see APR_STATUS_IS_ETIMEDOUT */
#ifdef ETIMEDOUT
#define APR_ETIMEDOUT ETIMEDOUT
#else
#define APR_ETIMEDOUT      (APR_OS_START_CANONERR + 20)
#endif

/** @see APR_STATUS_IS_EHOSTUNREACH */
#ifdef EHOSTUNREACH
#define APR_EHOSTUNREACH EHOSTUNREACH
#else
#define APR_EHOSTUNREACH   (APR_OS_START_CANONERR + 21)
#endif

/** @see APR_STATUS_IS_ENETUNREACH */
#ifdef ENETUNREACH
#define APR_ENETUNREACH ENETUNREACH
#else
#define APR_ENETUNREACH    (APR_OS_START_CANONERR + 22)
#endif

/** @see APR_STATUS_IS_EFTYPE */
#ifdef EFTYPE
#define APR_EFTYPE EFTYPE
#else
#define APR_EFTYPE        (APR_OS_START_CANONERR + 23)
#endif

/** @see APR_STATUS_IS_EPIPE */
#ifdef EPIPE
#define APR_EPIPE EPIPE
#else
#define APR_EPIPE         (APR_OS_START_CANONERR + 24)
#endif

/** @see APR_STATUS_IS_EXDEV */
#ifdef EXDEV
#define APR_EXDEV EXDEV
#else
#define APR_EXDEV         (APR_OS_START_CANONERR + 25)
#endif

/** @see APR_STATUS_IS_ENOTEMPTY */
#ifdef ENOTEMPTY
#define APR_ENOTEMPTY ENOTEMPTY
#else
#define APR_ENOTEMPTY     (APR_OS_START_CANONERR + 26)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* ! APR_ERRNO_H */
