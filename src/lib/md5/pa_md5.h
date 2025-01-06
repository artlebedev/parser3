/** @file
	taken from libmd, added pa_ prefixes, Parser 3 includes,
	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
*/

/*	$OpenBSD: md5.h,v 1.15 2004/05/03 17:30:14 millert Exp $	*/

/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest. This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 */

#ifndef PA_MD5_H
#define PA_MD5_H

#define IDENT_PA_MD5_H "$Id: pa_md5.h,v 1.19 2025/01/06 18:22:23 moko Exp $"

#ifdef __cplusplus
extern "C" {
#endif

#include "pa_config_includes.h"

#define	MD5_BLOCK_LENGTH		64
#define	MD5_DIGEST_LENGTH		16
#define	MD5_DIGEST_STRING_LENGTH	(MD5_DIGEST_LENGTH * 2 + 1)

typedef struct MD5Context {
	uint32_t state[4];			/* state */
	uint64_t count;				/* number of bits, mod 2^64 */
	uint8_t buffer[MD5_BLOCK_LENGTH];	/* input buffer */
} PA_MD5_CTX;

void pa_MD5Init(PA_MD5_CTX *);
void pa_MD5Update(PA_MD5_CTX *, const uint8_t *, size_t);
void pa_MD5Final(uint8_t [MD5_DIGEST_LENGTH], PA_MD5_CTX *);
void pa_MD5Transform(uint32_t [4], const uint8_t [MD5_BLOCK_LENGTH]);
void pa_MD5Encode(const unsigned char *password, const unsigned char *salt, char *result, size_t nbytes);
void pa_to64(char *s, unsigned long v, int n);

#define PA_MD5PW_ID "$apr1$"
#define PA_MD5PW_IDLEN 6

#ifdef __cplusplus
}
#endif

#endif	/* !PA_MD5_H */
