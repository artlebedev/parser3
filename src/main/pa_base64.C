/**	@file
	Parser: base64 functions impl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_base64.h"
#include "pa_common.h"

volatile const char * IDENT_PA_BASE64_C="$Id: pa_base64.C,v 1.11 2020/12/15 17:10:34 moko Exp $" IDENT_PA_BASE64_H;

/*
 *  BASE64 part inspired by g_mime_utils
 *  Authors: Michael Zucchi <notzed@ximian.com>
 *           Jeffrey Stedfast <fejj@ximian.com>
 *
 *  Copyright 2000-2004 Ximian, Inc. (www.ximian.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 *
 */

static const char *base64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char *base64_alphabet_url_safe = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

Base64Options::Base64Options(bool awrap): strict(false), wrap(awrap), pad(true), abc(base64_alphabet) {}

void Base64Options::set_url_safe_abc() {
	abc = base64_alphabet_url_safe;
}

#define BASE64_GROUPS_IN_LINE 19

static size_t pa_base64_encode(const uchar *in, size_t inlen, uchar *out, Base64Options options) {
	const uchar *inptr = in;
	uchar *outptr = out;

	const uchar *abc = (const uchar *)options.abc;

	if (inlen > 2) {
		const uchar *inend = in + inlen - 2;
		int already=0;
		
		while (inptr < inend) {
			int c1 = *inptr++;
			int c2 = *inptr++;
			int c3 = *inptr++;
			*outptr++ = abc[c1 >> 2];
			*outptr++ = abc[(c2 >> 4) | ((c1 & 0x3) << 4)];
			*outptr++ = abc[((c2 & 0x0f) << 2) | (c3 >> 6)];
			*outptr++ = abc[c3 & 0x3f];

			if ((++already) >= BASE64_GROUPS_IN_LINE && options.wrap) {
				*outptr++ = '\n';
				already = 0;
			}
		}
		
		inlen = 2 - (inptr - inend);
	}

	if (inlen == 2) {
		int c1 = *inptr++;
		int c2 = *inptr++;
		outptr[0] = abc[c1 >> 2];
		outptr[1] = abc[c2 >> 4 | ((c1 & 0x3) << 4)];
		outptr[2] = abc[(c2 & 0x0f) << 2];
		if(options.pad) {
			outptr[3] = '=';
			outptr += 4;
		} else {
			outptr += 3;
		}
	} else if (inlen == 1) {
		int c1 = *inptr++;
		outptr[0] = abc[c1 >> 2];
		outptr[1] = abc[(c1 & 0x3) << 4];
		if(options.pad) {
			outptr[2] = '=';
			outptr[3] = '=';
			outptr += 4;
		} else {
			outptr += 2;
		}
	}

	*outptr='\0';
	return outptr - out;
}

static uchar gmime_base64_rank[256] = {
	255,255,255,255,255,255,255,255,255,254,254,255,255,254,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	254,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,253,255,255,
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};

static uchar gmime_base64_rank_url_safe[256] = {
	255,255,255,255,255,255,255,255,255,254,254,255,255,254,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	254,255,255,255,255,255,255,255,255,255,255,255,255, 62,255,255,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,253,255,255,
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255, 63,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
};

size_t pa_base64_decode(const uchar *in, size_t inlen, uchar *out, Base64Options options) {
	const uchar *inptr = in;
	uchar *outptr = out;
	const uchar *inend = in + inlen;

	int saved = 0;
	int state = 0;
	
	uchar *abc_rank = options.abc == base64_alphabet ? gmime_base64_rank : gmime_base64_rank_url_safe;

	/* convert 4 base64 bytes to 3 normal bytes */
	while (inptr < inend) {
		uchar c = abc_rank[*inptr++];
		switch(c) {
			case 255: // non-base64 and non-whitespace chars. not allowed in strict mode
				if(options.strict)
					throw Exception(BASE64_FORMAT, 0, "Invalid base64 char on position %d is detected", inptr - in - 1);
			case 254: // whitespace chars 0x09, 0x0A, 0x0D, 0x20 are allowed in any mode
				break;
			case 253: // =
				if(state < 2) {
					if(options.strict)
						throw Exception(BASE64_FORMAT, 0, "Unexpected '=' on position %d is detected", inptr - in - 1);
					break;
				}
				if(state == 2) { // double '='
					if(inptr == inend) {
						if(options.strict)
							throw Exception(BASE64_FORMAT, 0, "Unexpected end of chars");
						break;
					}
					if(*inptr != '=') {
						if(options.strict)
							throw Exception(BASE64_FORMAT, 0, "Unexpected '=' on position %d is detected", inptr - in - 1);
						break;
					}
					inptr++;
					*outptr++ = (uchar)(saved >> 4);
				} else { // single '='
					*outptr++ = (uchar)(saved >> 10);
					*outptr++ = (uchar)(saved >> 2);
				}
				state = 0;
				break;
			default:
				saved = (saved << 6) | c;
				state++;
				if (state == 4) {
					*outptr++ = (uchar)(saved >> 16);
					*outptr++ = (uchar)(saved >> 8);
					*outptr++ = (uchar)(saved);
					state = 0;
				}
		}
	}

	if(state > 0) {
		if(state > 1) {
			if(options.pad && options.strict)
				throw Exception(BASE64_FORMAT, 0, "Unexpected end of chars");
			if(state == 2) {
				*outptr++ = (uchar)(saved >> 4);
			} else {
				*outptr++ = (uchar)(saved >> 10);
				*outptr++ = (uchar)(saved >> 2);
			}
		} else {
			if(options.strict)
				throw Exception(BASE64_FORMAT, 0, "Unexpected end of chars");
		}
	}

	*outptr='\0';  // for text files
	return outptr - out;
}

char* pa_base64_encode(const char *in, size_t in_size, Base64Options options) {
	size_t new_size = ((in_size / 3 + 1) * 4);
	if (options.wrap)
		new_size += new_size / (BASE64_GROUPS_IN_LINE * 4) /*new lines*/;

	char* result = new(PointerFreeGC) char[new_size + 1 /*zero terminator*/];

	size_t filled = pa_base64_encode((const uchar*)in, in_size, (uchar*)result, options);
	assert(filled <= new_size);

	return result;
}

size_t pa_base64_decode(const char *in, size_t in_size, char*& result, Base64Options options) {
	// every 4 base64 bytes are converted into 3 normal bytes
	size_t new_size = (in_size + 3) / 4 * 3;
	result = new(PointerFreeGC) char[new_size + 1 /*terminator*/];

	return pa_base64_decode((const uchar*)in, in_size, (uchar*)result, options);
}
