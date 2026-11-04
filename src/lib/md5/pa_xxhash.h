/** @file
	Parser: XXH32 and XXH64 non-cryptographic hash functions.

	The algorithms and their constants are by Yann Collet, xxHash (https://xxhash.com/):

	Copyright (C) 2012-2023 Yann Collet

	BSD 2-Clause License (https://www.opensource.org/licenses/bsd-license.php)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the following disclaimer
	  in the documentation and/or other materials provided with the
	  distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Compact portable implementation for Parser 3: works with any byte order and alignment
*/

#ifndef PA_XXHASH_H
#define PA_XXHASH_H

#define IDENT_PA_XXHASH_H "$Id: pa_xxhash.h,v 1.1 2026/11/04 13:40:43 moko Exp $"

#include "pa_config_includes.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t pa_xxh32(const void* input, size_t length, uint32_t seed);
uint64_t pa_xxh64(const void* input, size_t length, uint64_t seed);

#ifdef __cplusplus
}
#endif

#endif
