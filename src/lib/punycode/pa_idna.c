/** @file
   Parser: IDNA support, modified Libidn Version 1.28.
   Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

/* idna.c --- Prototypes for Internationalized Domain Name library.
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

#include "pa_punycode.h"
#include "pa_idna.h"

volatile const char * IDENT_PA_IDNA_C="$Id: pa_idna.c,v 1.7 2020/12/15 17:10:33 moko Exp $";

#define DOTP(c) ((c) == 0x002E || (c) == 0x3002 || (c) == 0xFF0E || (c) == 0xFF61)

#ifdef _MSC_VER
#	define strncasecmp _strnicmp
#endif

/* Core functions */

/**
 * idna_to_ascii_4z_internal:
 * @src: input array with unicode code points.
 * @len: length of input array with unicode code points.
 * @out: output zero terminated string that must have room for at
 *       least 63 characters plus the terminating zero.
 * @flags: an #Idna_flags value, e.g., %IDNA_ALLOW_UNASSIGNED or
 *   %IDNA_USE_STD3_ASCII_RULES.
 *
 * The ToASCII operation takes a sequence of Unicode code points that
 * make up one domain label and transforms it into a sequence of code
 * points in the ASCII range (0..7F). If ToASCII succeeds, the
 * original sequence and the resulting sequence are equivalent labels.
 *
 * It is important to note that the ToASCII operation can fail. ToASCII
 * fails if any step of it fails. If any step of the ToASCII operation
 * fails on any label in a domain name, that domain name MUST NOT be used
 * as an internationalized domain name. The method for deadling with this
 * failure is application-specific.
 *
 * The inputs to ToASCII are a sequence of code points, the AllowUnassigned
 * flag, and the UseSTD3ASCIIRules flag. The output of ToASCII is either a
 * sequence of ASCII code points or a failure condition.
 *
 * ToASCII never alters a sequence of code points that are all in the ASCII
 * range to begin with (although it could fail). Applying the ToASCII
 * operation multiple times has exactly the same effect as applying it just
 * once.
 *
 * Return value: Returns 0 on success, or an #Idna_rc error code.
 */

static int idna_to_ascii_4i_internal (const uint32_t *src, size_t len, char *out, int flags) {
	int rc;
	size_t out_len;

	/*
	 * 3. If the UseSTD3ASCIIRules flag is set, then perform these checks:
	 *
	 * (a) Verify the absence of non-LDH ASCII code points; that is,
	 * the absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
	 *
	 * (b) Verify the absence of leading and trailing hyphen-minus;
	 * that is, the absence of U+002D at the beginning and end of
	 * the sequence.
	 */

	if (flags & IDNA_USE_STD3_ASCII_RULES) {
		size_t i;

		for (i = 0; i < len; i++)
			if (src[i] <= 0x2C || src[i] == 0x2E || src[i] == 0x2F ||
				(src[i] >= 0x3A && src[i] <= 0x40) ||
				(src[i] >= 0x5B && src[i] <= 0x60) ||
				(src[i] >= 0x7B && src[i] <= 0x7F))
				return IDNA_CONTAINS_NON_LDH;

		if (src[0] == 0x002D || (i > 0 && src[i - 1] == 0x002D))
			return IDNA_CONTAINS_MINUS;
	}

	/*
	 * 4. If all code points in the sequence are in the ASCII range
	 * (0..7F), then skip to step 8.
	 */

	{
		size_t i;

		for (i = 0; i < len; i++) {
			if (src[i] > 0x7F)
				goto step5;
		}

		if (len > 63)
			return PUNYCODE_BIG_OUTPUT;

		/* copy string to output buffer if we are about to skip to step8 */
		for (i = 0; i < len; i++)
			out[i]=(char)src[i];
		out[len] = '\0';
		goto step8;
	}

	/*
	 * 5. Verify that the sequence does NOT begin with the ACE prefix.
	 */

step5:
	if (len >= strlen (IDNA_ACE_PREFIX)) {
		size_t i;

		for (i = 0; i < strlen (IDNA_ACE_PREFIX); i++)
			if (((uint32_t) IDNA_ACE_PREFIX[i] & 0xFF) != src[i])
				goto step6;
		 return IDNA_CONTAINS_ACE_PREFIX;
	}

	/*
	 * 6. Encode the sequence using the encoding algorithm in [PUNYCODE]
	 * and fail if there is an error.
	 */

step6:
	out_len = 63 - strlen (IDNA_ACE_PREFIX);
	rc = punycode_encode (len, src, NULL, &out_len, out + strlen(IDNA_ACE_PREFIX));

	if (rc != PUNYCODE_SUCCESS)
		return rc;

	out[strlen (IDNA_ACE_PREFIX) + out_len] = '\0';

	/*
	 * 7. Prepend the ACE prefix.
	 */

	memcpy (out, IDNA_ACE_PREFIX, strlen (IDNA_ACE_PREFIX));

step8:
	return IDNA_SUCCESS;
}

/**
 * idna_to_ascii_4z:
 * @in: zero terminated input Unicode string.
 * @out: pointer to output string.
 * @flags: an #Idna_flags value, e.g., %IDNA_ALLOW_UNASSIGNED or
 *   %IDNA_USE_STD3_ASCII_RULES.
 *
 * Convert UCS-4 domain name to ASCII string.  The domain name may
 * contain several labels, separated by dots.
 *
 * Return value: Returns %IDNA_SUCCESS on success, or error code.
 **/
int pa_idna_to_ascii_4z (const uint32_t *in, char *out, size_t out_len, int flags) {
	int rc;
	const uint32_t *start = in;
	const uint32_t *end;
	size_t add_len;
	char buf[64];

	/* 1) Whenever dots are used as label separators, the following
	   characters MUST be recognized as dots: U+002E (full stop),
	   U+3002 (ideographic full stop), U+FF0E (fullwidth full stop),
	   U+FF61 (halfwidth ideographic full stop). */

	do {
		for (end = start; *end && !DOTP (*end); end++);

		if (end > start) {
			rc = idna_to_ascii_4i_internal (start, (size_t) (end - start), buf, flags);
			if (rc != IDNA_SUCCESS)
				return rc;

			add_len = strlen (buf);
			if (add_len >= out_len)
				return PUNYCODE_BIG_OUTPUT;

			memcpy (out, buf, add_len);
			out += add_len;
			out_len -= add_len;
		}

		if (*end) {
			if (!out_len)
				return PUNYCODE_BIG_OUTPUT;
			*(out++)='.';
			out_len--;
		}

		start = end + 1;
	} while (*end);

	if (!out_len)
		return PUNYCODE_BIG_OUTPUT;

	*out='\0';

	return IDNA_SUCCESS;
}

/* ToUnicode(). */
static int idna_to_unicode_internal (const char *in, size_t in_len, uint32_t *out, size_t *out_len, int flags) {
	int rc;
	char tmpout[64];

	/* 3. Verify that the sequence begins with the ACE prefix
	 * ... The ToASCII and ToUnicode operations MUST recognize the ACE
	 prefix in a case-insensitive manner.
	 */

	if ( (in_len < strlen (IDNA_ACE_PREFIX)) || (strncasecmp (in, IDNA_ACE_PREFIX, strlen (IDNA_ACE_PREFIX)) != 0) ){
		size_t i;
		if (in_len >= *out_len)
			return PUNYCODE_BIG_OUTPUT;

		for(i=0; i<in_len; i++){
			if ((unsigned char)in[i] > 0x7F)
				return PUNYCODE_BAD_INPUT;
			out[i]=in[i];
		}
		*out_len=in_len;

		return IDNA_SUCCESS;
	} else {
		/* 4. Remove the ACE prefix.
		 */

		in += strlen (IDNA_ACE_PREFIX);
		in_len-=strlen (IDNA_ACE_PREFIX);

		/* 5. Decode the sequence using the decoding algorithm in [PUNYCODE]
		 * and fail if there is an error. Save a copy of the result of
		 * this step.
		 */

		rc = punycode_decode (in_len, in, out_len, out, NULL);
		if (rc != PUNYCODE_SUCCESS)
			return rc;

		/* 6. Apply ToASCII.
		 */

		rc = idna_to_ascii_4i_internal (out, *out_len, tmpout, flags);
		if (rc != IDNA_SUCCESS)
			return rc;

		/* 7. Verify that the result of step 6 matches the saved copy from
		 * step 3, using a case-insensitive ASCII comparison.
		 */

		if (strncasecmp (in, tmpout + strlen (IDNA_ACE_PREFIX), in_len) != 0)
			return IDNA_ROUNDTRIP_VERIFY_ERROR;

		/* 8. Return the saved copy from step 5.
		 */

		return IDNA_SUCCESS;
	}
}

/**
 * idna_to_unicode_4z:
 * @in: zero-terminated string.
 * @output: pointer to output Unicode string.
 * @flags: an #Idna_flags value, e.g., %IDNA_ALLOW_UNASSIGNED or
 *   %IDNA_USE_STD3_ASCII_RULES.
 *
 * Convert possibly ACE encoded domain name into a
 * UCS-4 string.  The domain name may contain several labels,
 * separated by dots.  The output buffer must be deallocated by the
 * caller.
 *
 * Return value: Returns %IDNA_SUCCESS on success, or error code.
 **/
int pa_idna_to_unicode_4z (const char *in, uint32_t *out, size_t out_len, int flags) {
	int rc;
	const char *start = in;
	const char *end;
	size_t add_len;

	do {
		for (end = start; *end && (*end != '.'); end++);

		if (end > start) {
			add_len=out_len;
			rc = idna_to_unicode_internal (start, (size_t) (end - start), out, &add_len, flags);
			if (rc != IDNA_SUCCESS)
				return rc;

			if (add_len >= out_len)
				return PUNYCODE_BIG_OUTPUT;

			out+=add_len;
			out_len-=add_len;
		}

		if (*end) {
			if (!out_len)
				return PUNYCODE_BIG_OUTPUT;
			*(out++) = 0x002E;	/* '.' (full stop) */
			out_len--;
		}

		start = end + 1;
	} while (*end);

	if (!out_len)
		return PUNYCODE_BIG_OUTPUT;

	*out=0;
	return IDNA_SUCCESS;
}

/**
 * IDNA_ACE_PREFIX
 *
 * The IANA allocated prefix to use for IDNA. "xn--"
 */

/**
 * Idna_rc:
 * @IDNA_SUCCESS: Successful operation.  This value is guaranteed to
 *   always be zero, the remaining ones are only guaranteed to hold
 *   non-zero values, for logical comparison purposes.
 * @IDNA_CONTAINS_NON_LDH: For IDNA_USE_STD3_ASCII_RULES, indicate that
 *   the string contains non-LDH ASCII characters.
 * @IDNA_CONTAINS_MINUS: For IDNA_USE_STD3_ASCII_RULES, indicate that
 *   the string contains a leading or trailing hyphen-minus (U+002D).
 * @IDNA_ROUNDTRIP_VERIFY_ERROR: The ToASCII operation on output
 *   string does not equal the input.
 * @IDNA_CONTAINS_ACE_PREFIX: The input contains the ACE prefix (for
 *   ToASCII).
 *
 * Enumerated return codes of idna_to_ascii_4i(),
 * idna_to_unicode_44i() functions (and functions derived from those
 * functions).  The value 0 is guaranteed to always correspond to
 * success.
 */

const char *pa_idna_strerror(int rc) {
	switch (rc) {
		case IDNA_SUCCESS:
			return "Success";
		case PUNYCODE_BAD_INPUT:
			return "Input is invalid";
		case PUNYCODE_BIG_OUTPUT:
			return "String is too long";
		case PUNYCODE_OVERFLOW:
			return "Wider integers needed to process input";
		case IDNA_CONTAINS_NON_LDH:
			return "Non-digit/letter/hyphen in input";
		case IDNA_CONTAINS_MINUS:
			return "Forbidden leading or trailing minus sign (`-')";
		case IDNA_ROUNDTRIP_VERIFY_ERROR:
			return "String not idempotent under ToASCII";
		case IDNA_CONTAINS_ACE_PREFIX:
			return "Input already contain ACE prefix (`xn--')";
		default:
			return "Unknown error";
	}
}

/**
 * Idna_flags:
 * @IDNA_ALLOW_UNASSIGNED: Don't reject strings containing unassigned
 *   Unicode code points.
 * @IDNA_USE_STD3_ASCII_RULES: Validate strings according to STD3
 *   rules (i.e., normal host name rules).
 *
 * Flags to pass to idna_to_ascii_4i(), idna_to_unicode_44i() etc.
 */
