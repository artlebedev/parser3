/*
 * FILE:	sha2.h
 * AUTHOR:	Aaron D. Gifford - http://www.aarongifford.com/
 * 
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: pa_sha2.h,v 1.2 2013/07/16 21:48:36 moko Exp $
 */

#ifndef __PA_SHA2_H__
#define __PA_SHA2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pa_config_includes.h"


/*** SHA-256/384/512 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH		64
#define SHA256_DIGEST_LENGTH		32
#define SHA256_DIGEST_STRING_LENGTH	(SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA384_BLOCK_LENGTH		128
#define SHA384_DIGEST_LENGTH		48
#define SHA384_DIGEST_STRING_LENGTH	(SHA384_DIGEST_LENGTH * 2 + 1)
#define SHA512_BLOCK_LENGTH		128
#define SHA512_DIGEST_LENGTH		64
#define SHA512_DIGEST_STRING_LENGTH	(SHA512_DIGEST_LENGTH * 2 + 1)


/*** SHA-256/384/512 Context Structures *******************************/
/* NOTE: If your architecture does not define either u_intXX_t types or
 * uintXX_t (from inttypes.h), you may need to define things by hand
 * for your system:
 */

typedef struct _SHA256_CTX {
	uint32_t	state[8];
	uint64_t	bitcount;
	uint8_t	buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;
typedef struct _SHA512_CTX {
	uint64_t	state[8];
	uint64_t	bitcount[2];
	uint8_t	buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;

typedef SHA512_CTX SHA384_CTX;


/*** SHA-256/384/512 Function Prototypes ******************************/

void pa_SHA256_Init(SHA256_CTX *);
void pa_SHA256_Update(SHA256_CTX*, const uint8_t*, size_t);
void pa_SHA256_Final(uint8_t[SHA256_DIGEST_LENGTH], SHA256_CTX*);
char* pa_SHA256_End(SHA256_CTX*, char[SHA256_DIGEST_STRING_LENGTH]);
char* pa_SHA256_Data(const uint8_t*, size_t, char[SHA256_DIGEST_STRING_LENGTH]);

void pa_SHA384_Init(SHA384_CTX*);
void pa_SHA384_Update(SHA384_CTX*, const uint8_t*, size_t);
void pa_SHA384_Final(uint8_t[SHA384_DIGEST_LENGTH], SHA384_CTX*);
char* pa_SHA384_End(SHA384_CTX*, char[SHA384_DIGEST_STRING_LENGTH]);
char* pa_SHA384_Data(const uint8_t*, size_t, char[SHA384_DIGEST_STRING_LENGTH]);

void pa_SHA512_Init(SHA512_CTX*);
void pa_SHA512_Update(SHA512_CTX*, const uint8_t*, size_t);
void pa_SHA512_Final(uint8_t[SHA512_DIGEST_LENGTH], SHA512_CTX*);
char* pa_SHA512_End(SHA512_CTX*, char[SHA512_DIGEST_STRING_LENGTH]);
char* pa_SHA512_Data(const uint8_t*, size_t, char[SHA512_DIGEST_STRING_LENGTH]);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif /* __SHA2_H__ */

