/** @file
	Parser: copied from apache 1.3.20  sources.
	Replaced ap_ to pa_ prefixes. linked into all targets but Apache-module target,
	where linked targets/apache/pa_md5c.c stub instead.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
*/

/*
 * This is work is derived from material Copyright RSA Data Security, Inc.
 *
 * The RSA copyright statement and Licence for that original material is
 * included below. This is followed by the Apache copyright statement and
 * licence for the modifications made to that material.
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

/* ====================================================================
 * Copyright (c) 1996-1999 The Apache Group.  All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

#ifndef PA_MD5_H
#define PA_MD5_H

static const char * const IDENT_MD5_H="$Date: 2005/08/09 08:14:51 $";

#define PA_API_EXPORT(rtype) rtype

#ifdef __cplusplus
extern "C" {
#endif

#ifndef APACHE_MD5_H

/* MD5.H - header file for MD5C.C */

#define MD5_DIGESTSIZE 16

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

#endif

/* MD5 context. */
typedef struct {
    UINT4 state[4];		/* state (ABCD) */
    UINT4 count[2];		/* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];	/* input buffer */
} PA_MD5_CTX;

/*
 * Define the Magic String prefix that identifies a password as being
 * hashed using our algorithm.
 */
#define PA_MD5PW_ID "$apr1$"
#define PA_MD5PW_IDLEN 6

PA_API_EXPORT(void) pa_MD5Init(PA_MD5_CTX *context);
PA_API_EXPORT(void) pa_MD5Update(PA_MD5_CTX *context, const unsigned char *input,
			      unsigned int inputLen);
PA_API_EXPORT(void) pa_MD5Final(unsigned char digest[MD5_DIGESTSIZE],
			     PA_MD5_CTX *context);
PA_API_EXPORT(void) pa_MD5Encode(const unsigned char *password,
			      const unsigned char *salt,
			      char *result, size_t nbytes);
PA_API_EXPORT(void) pa_to64(char *s, unsigned long v, int n);

#ifdef __cplusplus
}
#endif

#endif	/* !PA_MD5_H */
