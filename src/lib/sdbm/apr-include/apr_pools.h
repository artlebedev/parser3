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

#ifndef APR_POOLS_H
#define APR_POOLS_H

#include "apr_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file apr_pools.h
 * @brief APR memory allocation
 *
 * Resource allocation routines...
 *
 * designed so that we don't have to keep track of EVERYTHING so that
 * it can be explicitly freed later (a fundamentally unsound strategy ---
 * particularly in the presence of die()).
 *
 * Instead, we maintain pools, and allocate items (both memory and I/O
 * handlers) from the pools --- currently there are two, one for per
 * transaction info, and one for config info.  When a transaction is over,
 * we can delete everything in the per-transaction apr_pool_t without fear,
 * and without thinking too hard about it either.
 */

typedef void apr_pool_t;

/*
 * Cleanup
 */

/**
 * Register a function to be called when a pool is cleared or destroyed
 * @param p The pool register the cleanup with
 * @param data The data to pass to the cleanup function.
 * @param plain_cleanup The function to call when the pool is cleared
 *                      or destroyed
 * @param child_cleanup The function to call when a child process is being
 *                      shutdown - this function is called in the child, obviously!
 */
APR_DECLARE(void) apr_pool_cleanup_register(
    apr_pool_t *p,
    const void *data,
    apr_status_t (*plain_cleanup)(void *),
    apr_status_t (*child_cleanup)(void *));
/**
 * An empty cleanup function
 * @param data The data to cleanup
 */
APR_DECLARE_NONSTD(apr_status_t) apr_pool_cleanup_null(void *data);

/**
 * Run the specified cleanup function immediately and unregister it. Use
 * @a data instead of the data that was registered with the cleanup.
 * @param p The pool remove the cleanup from
 * @param data The data to remove from cleanup
 * @param cleanup The function to remove from cleanup
 */
APR_DECLARE(apr_status_t) apr_pool_cleanup_run(
    apr_pool_t *p,
    void *data,
    apr_status_t (*cleanup)(void *));

#ifdef __cplusplus
}
#endif

#endif /* !APR_POOLS_H */
