/**@TODO paf remove unneeded functions */

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

/* Portions of this file are covered by */
/* -*- mode: c; c-file-style: "k&r" -*-

  strnatcmp.c -- Perform 'natural order' comparisons of strings in C.
  Copyright (C) 2000 by Martin Pool <mbp@humbug.org.au>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef APR_STRINGS_H
#define APR_STRINGS_H

#include "apr.h"
#include "apr_errno.h"
#include "apr_pools.h"

#define HAVE_STDARG_H 1
#if HAVE_STDARG_H
#include <stdarg.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @file apr_strings.h
 * @brief APR Strings library
 */
/**
 * @defgroup APR_Strings String routines
 * @ingroup APR
 * @{
 */
/**
 * Do a natural order comparison of two strings.
 * @param a The first string to compare
 * @param b The second string to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *          this returns <0, if they are equivalent it returns 0, and if the
 *          first string is greater than second string it retuns >0.
 */
APR_DECLARE(int) apr_strnatcmp(char const *a, char const *b);

/**
 * Do a natural order comparison of two strings ignoring the case of the 
 * strings.
 * @param a The first string to compare
 * @param b The second string to compare
 * @return Either <0, 0, or >0.  If the first string is less than the second
 *         this returns <0, if they are equivalent it returns 0, and if the
 *         first string is greater than second string it retuns >0.
 */
APR_DECLARE(int) apr_strnatcasecmp(char const *a, char const *b);

/**
 * duplicate a string into memory allocated out of a pool
 * @param p The pool to allocate out of
 * @param s The string to duplicate
 * @return The new string
 */
APR_DECLARE(char *) apr_pstrdup(apr_pool_t *p, const char *s);

/**
 * Create a null-terminated string by making a copy of a sequence
 * of characters and appending a null byte
 * @param p The pool to allocate out of
 * @param s The block of characters to duplicate
 * @param n The number of characters to duplicate
 * @return The new string
 * @remark This is a faster alternative to apr_pstrndup, for use
 *         when you know that the string being duplicated really
 *         has 'n' or more characters.  If the string might contain
 *         fewer characters, use apr_pstrndup.
 */
APR_DECLARE(char *) apr_pstrmemdup(apr_pool_t *p, const char *s, apr_size_t n);

/**
 * duplicate the first n characters of a string into memory allocated 
 * out of a pool; the new string will be '\0'-terminated
 * @param p The pool to allocate out of
 * @param s The string to duplicate
 * @param n The number of characters to duplicate
 * @return The new string
 */
APR_DECLARE(char *) apr_pstrndup(apr_pool_t *p, const char *s, apr_size_t n);

/**
 * Duplicate a block of memory.
 *
 * @param p The pool to allocate from
 * @param m The memory to duplicate
 * @param n The number of bytes to duplicate
 * @return The new block of memory
 */
APR_DECLARE(void *) apr_pmemdup(apr_pool_t *p, const void *m, apr_size_t n);

/**
 * Concatenate multiple strings, allocating memory out a pool
 * @param p The pool to allocate out of
 * @param ... The strings to concatenate.  The final string must be NULL
 * @return The new string
 */
APR_DECLARE_NONSTD(char *) apr_pstrcat(apr_pool_t *p, ...);

/**
 * Concatenate multiple strings specified in a writev-style vector
 * @param p The pool from which to allocate
 * @param vec The strings to concatenate
 * @param nvec The number of strings to concatenate
 * @param nbytes (output) strlen of new string (pass in NULL to omit)
 * @return The new string
 */
APR_DECLARE(char *) apr_pstrcatv(apr_pool_t *p, const struct iovec *vec,
                                 apr_size_t nvec, apr_size_t *nbytes);

/**
 * printf-style style printing routine.  The data is output to a string 
 * allocated from a pool
 * @param p The pool to allocate out of
 * @param fmt The format of the string
 * @param ap The arguments to use while printing the data
 * @return The new string
 */
APR_DECLARE(char *) apr_pvsprintf(apr_pool_t *p, const char *fmt, va_list ap);

/**
 * copy n characters from src to dst
 * @param dst The destination string
 * @param src The source string
 * @param dst_size The space available in dst; dst always receives
 *                 null-termination, so if src is longer than
 *                 dst_size, the actual number of characters copied is
 *                 dst_size - 1.
 * @remark
 * <PRE>
 * We re-implement this function to implement these specific changes:
 *       1) strncpy() doesn't always null terminate and we want it to.
 *       2) strncpy() null fills, which is bogus, esp. when copy 8byte strings
 *          into 8k blocks.
 *       3) Instead of returning the pointer to the beginning of the
 *          destination string, we return a pointer to the terminating '\0'
 *          to allow us to check for truncation.
 * </PRE>
 */
APR_DECLARE(char *) apr_cpystrn(char *dst, const char *src,
                                apr_size_t dst_size);

/**
 * Strip spaces from a string
 * @param dest The destination string.  It is okay to modify the string
 *             in place.  Namely dest == src
 * @param src The string to rid the spaces from.
 */
APR_DECLARE(char *) apr_collapse_spaces(char *dest, const char *src);

/**
 * Convert the arguments to a program from one string to an array of 
 * strings terminated by a NULL pointer
 * @param str The arguments to convert
 * @param argv_out Output location.  This is a pointer to an array of strings.
 * @param token_context Pool to use.
 */
APR_DECLARE(apr_status_t) apr_tokenize_to_argv(const char *arg_str,
                                               char ***argv_out,
                                               apr_pool_t *token_context);

/**
 * Split a string into separate '\0'-terminated tokens.  The tokens are 
 * delimited in the string by one or more characters from the sep
 * argument.
 * @param str The string to separate; this should be specified on the
 *            first call to apr_strtok() for a given string, and NULL
 *            on subsequent calls.
 * @param sep The set of delimiters
 * @param last Internal state saved by apr_strtok() between calls.
 * @return The next token from the string
 */
APR_DECLARE(char *) apr_strtok(char *str, const char *sep, char **last);

/**
 * @defgroup APR_Strings_Snprintf snprintf implementations
 *
 * @warning
 * These are snprintf implementations based on apr_vformatter().
 *
 * Note that various standards and implementations disagree on the return
 * value of snprintf, and side-effects due to %n in the formatting string.
 * apr_snprintf (and apr_vsnprintf) behaves as follows:
 *
 * Process the format string until the entire string is exhausted, or
 * the buffer fills.  If the buffer fills then stop processing immediately
 * (so no further %n arguments are processed), and return the buffer
 * length.  In all cases the buffer is NUL terminated. It will return the
 * number of characters inserted into the buffer, not including the
 * terminating NUL. As a special case, if len is 0, apr_snprintf will
 * return the number of characters that would have been inserted if
 * the buffer had been infinite (in this case, *buffer can be NULL)
 *
 * In no event does apr_snprintf return a negative number.
 * @{
 */

/**
 * vsnprintf routine based on apr_vformatter.  This means it understands the
 * same extensions.
 * @param buf The buffer to write to
 * @param len The size of the buffer
 * @param format The format string
 * @param ap The arguments to use to fill out the format string.
 */
APR_DECLARE(int) apr_vsnprintf(char *buf, apr_size_t len, const char *format,
                               va_list ap);

/** @} */
/**
 * create a string representation of an int, allocated from a pool
 * @param p The pool from which to allocate
 * @param n The number to format
 * @return The string representation of the number
 */
APR_DECLARE(char *) apr_itoa(apr_pool_t *p, int n);

/**
 * create a string representation of a long, allocated from a pool
 * @param p The pool from which to allocate
 * @param n The number to format
 * @return The string representation of the number
 */
APR_DECLARE(char *) apr_ltoa(apr_pool_t *p, long n);

/**
 * create a string representation of an apr_off_t, allocated from a pool
 * @param p The pool from which to allocate
 * @param n The number to format
 * @return The string representation of the number
 */
APR_DECLARE(char *) apr_off_t_toa(apr_pool_t *p, apr_off_t n);

/**
 * Format a binary size (magnitiudes are 2^10 rather than 10^3) from an apr_off_t,
 * as bytes, K, M, T, etc, to a four character compacted human readable string.
 * @param size The size to format
 * @param buf The 5 byte text buffer (counting the trailing null)
 * @return The buf passed to apr_strfsize()
 * @remark All negative sizes report '  - ', apr_strfsize only formats positive values.
 */
APR_DECLARE(char *) apr_strfsize(apr_off_t size, char *buf);
/** @} */
#ifdef __cplusplus
}
#endif

#endif  /* !APR_STRINGS_H */
