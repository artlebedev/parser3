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

#ifndef APR_H
#define APR_H

#include "pa_config_includes.h"
#ifdef HAVE_ERRNO_H
#define APR_HAVE_ERRNO_H 1
#endif

#include <stddef.h>

typedef  int               apr_int32_t;

/**
 * APR_DECLARE_EXPORT is defined when building the APR dynamic library,
 * so that all public symbols are exported.
 *
 * APR_DECLARE_STATIC is defined when including the APR public headers,
 * to provide static linkage when the dynamic library may be unavailable.
 *
 * APR_DECLARE_STATIC and APR_DECLARE_EXPORT are left undefined when
 * including the APR public headers, to import and link the symbols from the 
 * dynamic APR library and assure appropriate indirection and calling 
 * conventions at compile time.
 */

/**
 * The public APR functions are declared with APR_DECLARE(), so they may
 * use the most appropriate calling convention.  Public APR functions with 
 * variable arguments must use APR_DECLARE_NONSTD().
 *
 * @deffunc APR_DECLARE(rettype) apr_func(args);
 */
#define APR_DECLARE(type)            type

/**
 * The public APR functions using variable arguments are declared with 
 * AP_DECLARE(), as they must use the C language calling convention.
 *
 * @deffunc APR_DECLARE_NONSTD(rettype) apr_func(args, ...);
 */
#define APR_DECLARE_NONSTD(type)     type

/**
 * The public APR variables are declared with AP_MODULE_DECLARE_DATA.
 * This assures the appropriate indirection is invoked at compile time.
 *
 * @deffunc APR_DECLARE_DATA type apr_variable;
 * @tip APR_DECLARE_DATA extern type apr_variable; syntax is required for
 * declarations within headers to properly import the variable.
 */
#define APR_DECLARE_DATA

typedef  int         apr_off_t;
typedef size_t apr_size_t;

#endif /* APR_H */

