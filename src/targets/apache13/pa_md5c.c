/** @file
	Parser: stub to pass MD5 func calls to apache 1.3 core.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_MD5C_C="$Date: 2003/07/24 08:19:21 $";

#include "ap_config.h"
#include "ap_md5.h"

#include "pa_md5.h"

void PA_MD5Init(PA_MD5_CTX *context) { ap_MD5Init((AP_MD5_CTX*)context); }
void PA_MD5Update(PA_MD5_CTX *context, const unsigned char *input,
		  unsigned int inputLen) { ap_MD5Update((AP_MD5_CTX *)context, input, inputLen); }
void PA_MD5Final(unsigned char digest[MD5_DIGESTSIZE],
		 PA_MD5_CTX *context) { ap_MD5Final(digest, (AP_MD5_CTX *)context); }

void PA_MD5Encode(const unsigned char *password,
		  const unsigned char *salt,
		  char *result, size_t nbytes) { ap_MD5Encode(password, salt, result, nbytes); }
void PA_to64(char *s, unsigned long v, int n) { ap_to64(s, v, n); }
