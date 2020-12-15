/** @file
   Parser: IDNA support, modified Libidn Version 1.28.
   Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

/* idna.h --- Prototypes for Internationalized Domain Name library.
   Copyright (C) 2002-2013 Simon Josefsson

   This file is part of GNU Libidn.

   GNU Libidn is free software: you can redistribute it and/or
   modify it under the terms of either:

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version.

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version.

   or both in parallel, as here.

   GNU Libidn is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>. */

#ifndef PA_IDNA_H
#define PA_IDNA_H

#include "pa_config_includes.h"

# ifdef __cplusplus
extern "C"
{
# endif

/* Error codes. */
typedef enum {
	IDNA_SUCCESS = 0,
	IDNA_CONTAINS_NON_LDH = 4,
	IDNA_CONTAINS_MINUS = 5,
	IDNA_NO_ACE_PREFIX = 6,
	IDNA_ROUNDTRIP_VERIFY_ERROR = 7,
	IDNA_CONTAINS_ACE_PREFIX = 8
} Idna_rc;

/* IDNA flags */
typedef enum {
	IDNA_ALLOW_UNASSIGNED = 0x0001,
	IDNA_USE_STD3_ASCII_RULES = 0x0002
} Idna_flags;

#ifndef IDNA_ACE_PREFIX
#define IDNA_ACE_PREFIX "xn--"
#endif

const char *pa_idna_strerror (int rc);

int pa_idna_to_ascii_4z (const uint32_t *in, char *out, size_t out_len, int flags);
int pa_idna_to_unicode_4z (const char *in, uint32_t *out, size_t out_len, int flags);

# ifdef __cplusplus
}
# endif

#endif /* PA_IDNA_H */
