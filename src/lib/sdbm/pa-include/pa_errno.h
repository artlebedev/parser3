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
 * PA_OS_START_USEERR is obsolete, defined for compatibility only.
 * Use PA_OS_START_USERERR instead.
 */
#define PA_OS_START_USEERR     PA_OS_START_USERERR
/**
 * PA_OS_START_CANONERR is where APR versions of errno values are defined
 *     on systems which don't have the corresponding errno.
 */
#define PA_OS_START_CANONERR  (PA_OS_START_USERERR \
                                 + (PA_OS_ERRSPACE_SIZE * 10))
/**
 * PA_OS_START_EAIERR folds EAI_ error codes from getaddrinfo() into 
 *     pa_status_t values.
 */
#define PA_OS_START_EAIERR    (PA_OS_START_CANONERR + PA_OS_ERRSPACE_SIZE)
/**
 * PA_OS_START_SYSERR folds platform-specific system error values into 
 *     pa_status_t values.
 */
#define PA_OS_START_SYSERR    (PA_OS_START_EAIERR + PA_OS_ERRSPACE_SIZE)

/** no error. @see PA_STATUS_IS_SUCCESS */
#define PA_SUCCESS 0

/* APR ERROR VALUES */
/** 
 * @defgroup APRErrorValues Error Values
 * <PRE>
 * <b>APR ERROR VALUES</b>
 * PA_ENOSTAT      APR was unable to perform a stat on the file 
 * PA_ENOPOOL      APR was not provided a pool with which to allocate memory
 * PA_EBADDATE     APR was given an invalid date 
 * PA_EINVALSOCK   APR was given an invalid socket
 * PA_ENOPROC      APR was not given a process structure
 * PA_ENOTIME      APR was not given a time structure
 * PA_ENODIR       APR was not given a directory structure
 * PA_ENOLOCK      APR was not given a lock structure
 * PA_ENOPOLL      APR was not given a poll structure
 * PA_ENOSOCKET    APR was not given a socket
 * PA_ENOTHREAD    APR was not given a thread structure
 * PA_ENOTHDKEY    APR was not given a thread key structure
 * PA_ENOSHMAVAIL  There is no more shared memory available
 * PA_EDSOOPEN     APR was unable to open the dso object.  For more 
 *                  information call pa_dso_error().
 * PA_EGENERAL     General failure (specific information not available)
 * PA_EBADIP       The specified IP address is invalid
 * PA_EBADMASK     The specified netmask is invalid
 * </PRE>
 *
 * <PRE>
 * <b>APR STATUS VALUES</b>
 * PA_INCHILD        Program is currently executing in the child
 * PA_INPARENT       Program is currently executing in the parent
 * PA_DETACH         The thread is detached
 * PA_NOTDETACH      The thread is not detached
 * PA_CHILD_DONE     The child has finished executing
 * PA_CHILD_NOTDONE  The child has not finished executing
 * PA_TIMEUP         The operation did not finish before the timeout
 * PA_INCOMPLETE     The operation was incomplete although some processing
 *                    was performed and the results are partially valid
 * PA_BADCH          Getopt found an option not in the option string
 * PA_BADARG         Getopt found an option that is missing an argument 
 *                    and an argument was specified in the option string
 * PA_EOF            APR has encountered the end of the file
 * PA_NOTFOUND       APR was unable to find the socket in the poll structure
 * PA_ANONYMOUS      APR is using anonymous shared memory
 * PA_FILEBASED      APR is using a file name as the key to the shared memory
 * PA_KEYBASED       APR is using a shared key as the key to the shared memory
 * PA_EINIT          Ininitalizer value.  If no option has been found, but 
 *                    the status variable requires a value, this should be used
 * PA_ENOTIMPL       The APR function has not been implemented on this 
 *                    platform, either because nobody has gotten to it yet, 
 *                    or the function is impossible on this platform.
 * PA_EMISMATCH      Two passwords do not match.
 * PA_EABSOLUTE      The given path was absolute.
 * PA_ERELATIVE      The given path was relative.
 * PA_EINCOMPLETE    The given path was neither relative nor absolute.
 * PA_EABOVEROOT     The given path was above the root path.
 * PA_EBUSY          The given lock was busy.
 * </PRE>
 * @{
 */
/** @see PA_STATUS_IS_ENOSTAT */
#define PA_ENOSTAT        (PA_OS_START_ERROR + 1)
/** @see PA_STATUS_IS_ENOPOOL */
#define PA_ENOPOOL        (PA_OS_START_ERROR + 2)
/* empty slot: +3 */
/** @see PA_STATUS_IS_EBADDATE */
#define PA_EBADDATE       (PA_OS_START_ERROR + 4)
/** @see PA_STATUS_IS_EINVALSOCK */
#define PA_EINVALSOCK     (PA_OS_START_ERROR + 5)
/** @see PA_STATUS_IS_ENOPROC */
#define PA_ENOPROC        (PA_OS_START_ERROR + 6)
/** @see PA_STATUS_IS_ENOTIME */
#define PA_ENOTIME        (PA_OS_START_ERROR + 7)
/** @see PA_STATUS_IS_ENODIR */
#define PA_ENODIR         (PA_OS_START_ERROR + 8)
/** @see PA_STATUS_IS_ENOLOCK */
#define PA_ENOLOCK        (PA_OS_START_ERROR + 9)
/** @see PA_STATUS_IS_ENOPOLL */
#define PA_ENOPOLL        (PA_OS_START_ERROR + 10)
/** @see PA_STATUS_IS_ENOSOCKET */
#define PA_ENOSOCKET      (PA_OS_START_ERROR + 11)
/** @see PA_STATUS_IS_ENOTHREAD */
#define PA_ENOTHREAD      (PA_OS_START_ERROR + 12)
/** @see PA_STATUS_IS_ENOTHDKEY */
#define PA_ENOTHDKEY      (PA_OS_START_ERROR + 13)
/** @see PA_STATUS_IS_EGENERAL */
#define PA_EGENERAL       (PA_OS_START_ERROR + 14)
/** @see PA_STATUS_IS_ENOSHMAVAIL */
#define PA_ENOSHMAVAIL    (PA_OS_START_ERROR + 15)
/** @see PA_STATUS_IS_EBADIP */
#define PA_EBADIP         (PA_OS_START_ERROR + 16)
/** @see PA_STATUS_IS_EBADMASK */
#define PA_EBADMASK       (PA_OS_START_ERROR + 17)
/* empty slot: +18 */
/** @see PA_STATUS_IS_EDSOPEN */
#define PA_EDSOOPEN       (PA_OS_START_ERROR + 19)
/** @see PA_STATUS_IS_EABSOLUTE */
#define PA_EABSOLUTE      (PA_OS_START_ERROR + 20)
/** @see PA_STATUS_IS_ERELATIVE */
#define PA_ERELATIVE      (PA_OS_START_ERROR + 21)
/** @see PA_STATUS_IS_EINCOMPLETE */
#define PA_EINCOMPLETE    (PA_OS_START_ERROR + 22)
/** @see PA_STATUS_IS_EABOVEROOT */
#define PA_EABOVEROOT     (PA_OS_START_ERROR + 23)
/** @see PA_STATUS_IS_EBADPATH */
#define PA_EBADPATH       (PA_OS_START_ERROR + 24)

/* APR STATUS VALUES */
/** @see PA_STATUS_IS_INCHILD */
#define PA_INCHILD        (PA_OS_START_STATUS + 1)
/** @see PA_STATUS_IS_INPARENT */
#define PA_INPARENT       (PA_OS_START_STATUS + 2)
/** @see PA_STATUS_IS_DETACH */
#define PA_DETACH         (PA_OS_START_STATUS + 3)
/** @see PA_STATUS_IS_NOTDETACH */
#define PA_NOTDETACH      (PA_OS_START_STATUS + 4)
/** @see PA_STATUS_IS_CHILD_DONE */
#define PA_CHILD_DONE     (PA_OS_START_STATUS + 5)
/** @see PA_STATUS_IS_CHILD_NOTDONE */
#define PA_CHILD_NOTDONE  (PA_OS_START_STATUS + 6)
/** @see PA_STATUS_IS_TIMEUP */
#define PA_TIMEUP         (PA_OS_START_STATUS + 7)
/** @see PA_STATUS_IS_INCOMPLETE */
#define PA_INCOMPLETE     (PA_OS_START_STATUS + 8)
/* empty slot: +9 */
/* empty slot: +10 */
/* empty slot: +11 */
/** @see PA_STATUS_IS_BADCH */
#define PA_BADCH          (PA_OS_START_STATUS + 12)
/** @see PA_STATUS_IS_BADARG */
#define PA_BADARG         (PA_OS_START_STATUS + 13)
/** @see PA_STATUS_IS_EOF */
#define PA_EOF            (PA_OS_START_STATUS + 14)
/** @see PA_STATUS_IS_NOTFOUND */
#define PA_NOTFOUND       (PA_OS_START_STATUS + 15)
/* empty slot: +16 */
/* empty slot: +17 */
/* empty slot: +18 */
/** @see PA_STATUS_IS_ANONYMOUS */
#define PA_ANONYMOUS      (PA_OS_START_STATUS + 19)
/** @see PA_STATUS_IS_FILEBASED */
#define PA_FILEBASED      (PA_OS_START_STATUS + 20)
/** @see PA_STATUS_IS_KEYBASED */
#define PA_KEYBASED       (PA_OS_START_STATUS + 21)
/** @see PA_STATUS_IS_EINIT */
#define PA_EINIT          (PA_OS_START_STATUS + 22)  
/** @see PA_STATUS_IS_ENOTIMPL */
#define PA_ENOTIMPL       (PA_OS_START_STATUS + 23)
/** @see PA_STATUS_IS_EMISMATCH */
#define PA_EMISMATCH      (PA_OS_START_STATUS + 24)
/** @see PA_STATUS_IS_EBUSY */
#define PA_EBUSY          (PA_OS_START_STATUS + 25)

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

/** @see PA_STATUS_IS_ENAMETOOLONG */
#ifdef ENAMETOOLONG
#define PA_ENAMETOOLONG ENAMETOOLONG
#else
#define PA_ENAMETOOLONG   (PA_OS_START_CANONERR + 3)
#endif

/** @see PA_STATUS_IS_ENOENT */
#ifdef ENOENT
#define PA_ENOENT ENOENT
#else
#define PA_ENOENT         (PA_OS_START_CANONERR + 4)
#endif

/** @see PA_STATUS_IS_ENOTDIR */
#ifdef ENOTDIR
#define PA_ENOTDIR ENOTDIR
#else
#define PA_ENOTDIR        (PA_OS_START_CANONERR + 5)
#endif

/** @see PA_STATUS_IS_ENOSPC */
#ifdef ENOSPC
#define PA_ENOSPC ENOSPC
#else
#define PA_ENOSPC         (PA_OS_START_CANONERR + 6)
#endif

/** @see PA_STATUS_IS_ENOMEM */
#ifdef ENOMEM
#define PA_ENOMEM ENOMEM
#else
#define PA_ENOMEM         (PA_OS_START_CANONERR + 7)
#endif

/** @see PA_STATUS_IS_EMFILE */
#ifdef EMFILE
#define PA_EMFILE EMFILE
#else
#define PA_EMFILE         (PA_OS_START_CANONERR + 8)
#endif

/** @see PA_STATUS_IS_ENFILE */
#ifdef ENFILE
#define PA_ENFILE ENFILE
#else
#define PA_ENFILE         (PA_OS_START_CANONERR + 9)
#endif

/** @see PA_STATUS_IS_EBADF */
#ifdef EBADF
#define PA_EBADF EBADF
#else
#define PA_EBADF          (PA_OS_START_CANONERR + 10)
#endif

/** @see PA_STATUS_IS_EINVAL */
#ifdef EINVAL
#define PA_EINVAL EINVAL
#else
#define PA_EINVAL         (PA_OS_START_CANONERR + 11)
#endif

/** @see PA_STATUS_IS_ESPIPE */
#ifdef ESPIPE
#define PA_ESPIPE ESPIPE
#else
#define PA_ESPIPE         (PA_OS_START_CANONERR + 12)
#endif

/** 
 * @see PA_STATUS_IS_EAGAIN 
 * @warning use PA_STATUS_IS_EAGAIN instead of just testing this value
 */
#ifdef EAGAIN
#define PA_EAGAIN EAGAIN
#elif defined(EWOULDBLOCK)
#define PA_EAGAIN EWOULDBLOCK
#else
#define PA_EAGAIN         (PA_OS_START_CANONERR + 13)
#endif

/** @see PA_STATUS_IS_EINTR */
#ifdef EINTR
#define PA_EINTR EINTR
#else
#define PA_EINTR          (PA_OS_START_CANONERR + 14)
#endif

/** @see PA_STATUS_IS_ENOTSOCK */
#ifdef ENOTSOCK
#define PA_ENOTSOCK ENOTSOCK
#else
#define PA_ENOTSOCK       (PA_OS_START_CANONERR + 15)
#endif

/** @see PA_STATUS_IS_ECONNREFUSED */
#ifdef ECONNREFUSED
#define PA_ECONNREFUSED ECONNREFUSED
#else
#define PA_ECONNREFUSED   (PA_OS_START_CANONERR + 16)
#endif

/** @see PA_STATUS_IS_EINPROGRESS */
#ifdef EINPROGRESS
#define PA_EINPROGRESS EINPROGRESS
#else
#define PA_EINPROGRESS    (PA_OS_START_CANONERR + 17)
#endif

/** 
 * @see PA_STATUS_IS_ECONNABORTED
 * @warning use PA_STATUS_IS_ECONNABORTED instead of just testing this value
 */

#ifdef ECONNABORTED
#define PA_ECONNABORTED ECONNABORTED
#else
#define PA_ECONNABORTED   (PA_OS_START_CANONERR + 18)
#endif

/** @see PA_STATUS_IS_ECONNRESET */
#ifdef ECONNRESET
#define PA_ECONNRESET ECONNRESET
#else
#define PA_ECONNRESET     (PA_OS_START_CANONERR + 19)
#endif

/** @see PA_STATUS_IS_ETIMEDOUT */
#ifdef ETIMEDOUT
#define PA_ETIMEDOUT ETIMEDOUT
#else
#define PA_ETIMEDOUT      (PA_OS_START_CANONERR + 20)
#endif

/** @see PA_STATUS_IS_EHOSTUNREACH */
#ifdef EHOSTUNREACH
#define PA_EHOSTUNREACH EHOSTUNREACH
#else
#define PA_EHOSTUNREACH   (PA_OS_START_CANONERR + 21)
#endif

/** @see PA_STATUS_IS_ENETUNREACH */
#ifdef ENETUNREACH
#define PA_ENETUNREACH ENETUNREACH
#else
#define PA_ENETUNREACH    (PA_OS_START_CANONERR + 22)
#endif

/** @see PA_STATUS_IS_EFTYPE */
#ifdef EFTYPE
#define PA_EFTYPE EFTYPE
#else
#define PA_EFTYPE        (PA_OS_START_CANONERR + 23)
#endif

/** @see PA_STATUS_IS_EPIPE */
#ifdef EPIPE
#define PA_EPIPE EPIPE
#else
#define PA_EPIPE         (PA_OS_START_CANONERR + 24)
#endif

/** @see PA_STATUS_IS_EXDEV */
#ifdef EXDEV
#define PA_EXDEV EXDEV
#else
#define PA_EXDEV         (PA_OS_START_CANONERR + 25)
#endif

/** @see PA_STATUS_IS_ENOTEMPTY */
#ifdef ENOTEMPTY
#define PA_ENOTEMPTY ENOTEMPTY
#else
#define PA_ENOTEMPTY     (PA_OS_START_CANONERR + 26)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* ! PA_ERRNO_H */
