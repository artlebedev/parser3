/**	@file
	Parser: base64 functions impl.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_base64.h"
#include "pa_common.h"

volatile const char * IDENT_PA_BASE64_C="$Id: pa_base64.C,v 1.5 2019/11/15 21:48:51 moko Exp $" IDENT_PA_BASE64_H;

/*
 * BASE64 part
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

Base64Options::Base64Options(bool awrap): strict(false), wrap(awrap), pad(false), abc(base64_alphabet) {}

void Base64Options::set_url_safe_abc() {
	abc = base64_alphabet_url_safe;
}

/**
 * g_mime_utils_base64_encode_step:
 * @in: input stream
 * @inlen: length of the input
 * @out: output string
 * @state: holds the number of bits that are stored in @save
 * @save: leftover bits that have not yet been encoded
 *
 * Base64 encodes a chunk of data. Performs an 'encode step', only
 * encodes blocks of 3 characters to the output at a time, saves
 * left-over state in state and save (initialise to 0 on first
 * invocation).
 *
 * Returns the number of bytes encoded.
 **/

#define BASE64_GROUPS_IN_LINE 19

static size_t g_mime_utils_base64_encode_step (const unsigned char *in, size_t inlen, unsigned char *out, int *state, int *save) {
	register const unsigned char *inptr;
	register unsigned char *outptr;
	
	if (inlen <= 0)
		return 0;
	
	inptr = in;
	outptr = out;
	
	if (inlen + ((unsigned char *)save)[0] > 2) {
		const unsigned char *inend = in + inlen - 2;
		register int c1 = 0, c2 = 0, c3 = 0;
		register int already;
		
		already = *state;
		
		switch (((char *)save)[0]) {
		case 1:	c1 = ((unsigned char *)save)[1]; goto skip1;
		case 2:	c1 = ((unsigned char *)save)[1];
			c2 = ((unsigned char *)save)[2]; goto skip2;
		}
		
		/* yes, we jump into the loop, no i'm not going to change it, its beautiful! */
		while (inptr < inend) {
			c1 = *inptr++;
		skip1:
			c2 = *inptr++;
		skip2:
			c3 = *inptr++;
			*outptr++ = base64_alphabet [c1 >> 2];
			*outptr++ = base64_alphabet [(c2 >> 4) | ((c1 & 0x3) << 4)];
			*outptr++ = base64_alphabet [((c2 & 0x0f) << 2) | (c3 >> 6)];
			*outptr++ = base64_alphabet [c3 & 0x3f];
			/* this is a bit ugly ... */
			if ((++already) >= BASE64_GROUPS_IN_LINE) {
				*outptr++ = '\n';
				already = 0;
			}
		}
		
		((unsigned char *)save)[0] = 0;
		inlen = 2 - (inptr - inend);
		*state = already;
	}
	
	//d(printf ("state = %d, inlen = %d\n", (int)((char *)save)[0], inlen));
	
	if (inlen > 0) {
		register char *saveout;
		
		/* points to the slot for the next char to save */
		saveout = & (((char *)save)[1]) + ((char *)save)[0];
		
		/* inlen can only be 0 1 or 2 */
		switch (inlen) {
		case 2:	*saveout++ = *inptr++;
		case 1:	*saveout++ = *inptr++;
		}
		*(char *)save = *(char *)save+(char)inlen;
	}
	
	/*d(printf ("mode = %d\nc1 = %c\nc2 = %c\n",
		  (int)((char *)save)[0],
		  (int)((char *)save)[1],
		  (int)((char *)save)[2]));*/
	
	return (outptr - out);
}

/**
 * g_mime_utils_base64_encode_close:
 * @in: input stream
 * @inlen: length of the input
 * @out: output string
 * @state: holds the number of bits that are stored in @save
 * @save: leftover bits that have not yet been encoded
 *
 * Base64 encodes the input stream to the output stream. Call this
 * when finished encoding data with g_mime_utils_base64_encode_step to
 * flush off the last little bit.
 *
 * Returns the number of bytes encoded.
 **/
static size_t g_mime_utils_base64_encode_close (const unsigned char *in, size_t inlen, unsigned char *out, int *state, int *save) {
	unsigned char *outptr = out;
	int c1, c2;
	
	if (inlen > 0)
		outptr += g_mime_utils_base64_encode_step (in, inlen, outptr, state, save);
	
	c1 = ((unsigned char *)save)[1];
	c2 = ((unsigned char *)save)[2];
	
	switch (((unsigned char *)save)[0]) {
	case 2:
		outptr[2] = base64_alphabet [(c2 & 0x0f) << 2];
		goto skip;
	case 1:
		outptr[2] = '=';
	skip:
		outptr[0] = base64_alphabet [c1 >> 2];
		outptr[1] = base64_alphabet [c2 >> 4 | ((c1 & 0x3) << 4)];
		outptr[3] = '=';
		outptr += 4;
		break;
	}
	
	*outptr++ = 0;
	
	*save = 0;
	*state = 0;
	
	return (outptr - out);
}

static unsigned char gmime_base64_rank[256] = {
	255,255,255,255,255,255,255,255,255,254,254,255,255,254,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	254,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
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

static unsigned char gmime_base64_rank_url_safe[256] = {
	255,255,255,255,255,255,255,255,255,254,254,255,255,254,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	254,255,255,255,255,255,255,255,255,255,255,255,255, 62,255,255,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
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

/**
 * g_mime_utils_base64_decode_step:
 * @in: input stream
 * @inlen: max length of data to decode
 * @out: output stream
 * @strict: only base64 and whitespace chars are allowed
 *
 * Decodes a chunk of base64 encoded data.
 *
 * Returns the number of bytes decoded (which have been dumped in @out).
 **/
size_t g_mime_utils_base64_decode(const unsigned char *in, size_t inlen, unsigned char *out, Base64Options options) {
	const unsigned char *inptr = in;
	unsigned char *outptr = out;
	const unsigned char *inend = in + inlen;

	int saved = 0;
	int state = 0;
	
	unsigned char *abc_rank = options.abc == base64_alphabet ? gmime_base64_rank : gmime_base64_rank_url_safe;

	/* convert 4 base64 bytes to 3 normal bytes */
	while (inptr < inend) {
		unsigned char c = abc_rank[*inptr++];
		switch(c) {
			case 0xff: // non-base64 and non-whitespace chars. not allowed in strict mode
				if(options.strict)
					throw Exception(BASE64_FORMAT, 0, "Invalid base64 char on position %d is detected", inptr - in - 1);
			case 0xfe: // whitespace chars 0x09, 0x0A, 0x0D, 0x20 are allowed in any mode
				break;
			default:
				saved = (saved << 6) | c;
				state++;
				if (state == 4) {
					*outptr++ = (unsigned char)(saved >> 16);
					*outptr++ = (unsigned char)(saved >> 8);
					*outptr++ = (unsigned char)(saved);
					state = 0;
				}
		}
	}
	
	if(options.strict && state !=0 )
		throw Exception(BASE64_FORMAT, 0, "Unexpected end of chars");

	/* quick scan back for '=' on the end somewhere */
	/* fortunately we can drop 1 output char for each trailing = (upto 2) */
	state = 2;
	while (inptr > in && state) {
		inptr--;
		if (abc_rank[*inptr] <= 0xfe) {
			if (*inptr == '=' && outptr > out)
				outptr--;
			state--;
		}
	}

	/* if state != 0 then there is a truncation error! */
	return (outptr - out);
}

size_t pa_base64_size(size_t in_size, bool wrap){
	size_t new_size = ((in_size / 3 + 1) * 4) + 1 /*zero terminator*/;
	if (wrap) new_size += new_size / (BASE64_GROUPS_IN_LINE * 4) /*new lines*/;
	return new_size;
}

char* pa_base64_encode(const char *in, size_t in_size, Base64Options options) {
	size_t new_size = pa_base64_size(in_size, options.wrap);
	char* result = new(PointerFreeGC) char[new_size];
	int state = 0;
	int save = 0;
	size_t filled = g_mime_utils_base64_encode_close ((const unsigned char*)in, in_size, (unsigned char*)result, &state, &save);

	//throw Exception(PARSER_RUNTIME, 0, "%d %d %d", in_size, new_size, filled);
	assert(filled <= new_size);

	return result;
}

struct File_base64_action_info {
	unsigned char** base64;
}; 

static void file_base64_file_action(struct stat& finfo, int f, const String& file_spec, void *context) {
	if(finfo.st_size) {
		File_base64_action_info& info = *static_cast<File_base64_action_info *>(context);
		*info.base64 = new(PointerFreeGC) unsigned char[pa_base64_size(check_file_size(finfo.st_size, file_spec), true)]; 
		unsigned char* base64 = *info.base64;
		int state = 0;
		int save = 0;
		int nCount;
		do {
			unsigned char buffer[FILE_BUFFER_SIZE];
			nCount = file_block_read(f, buffer, sizeof(buffer));
			if( nCount ){
				size_t filled = g_mime_utils_base64_encode_step ((const unsigned char*)buffer, nCount, base64, &state, &save);
				base64 += filled;
			}
		} while(nCount > 0);
		g_mime_utils_base64_encode_close (0, 0, base64, &state, &save);
	}
}

char* pa_base64_encode(const String& file_spec, Base64Options options){
	unsigned char* base64 = 0;
	File_base64_action_info info = { &base64 };

	file_read_action_under_lock(file_spec, "pa_base64_encode", file_base64_file_action, &info);

	return (char*)base64; 
}

void pa_base64_decode(const char *in, size_t in_size, char*& result, size_t& result_size, Base64Options options) {
	// every 4 base64 bytes are converted into 3 normal bytes
	// not full set (tail) of 4-bytes set is ignored
	size_t new_size = (in_size + 3) / 4 * 3;
	result = new(PointerFreeGC) char[new_size + 1 /*terminator*/];

	result_size = g_mime_utils_base64_decode ((const unsigned char*)in, in_size, (unsigned char*)result, options);
	assert(result_size <= new_size);
	result[result_size] = 0; // for text files

}

