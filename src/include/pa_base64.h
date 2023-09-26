/** @file
	Parser: base64 functions.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_BASE64_H
#define PA_BASE64_H

#define IDENT_PA_BASE64_H "$Id: pa_base64.h,v 1.7 2023/09/26 20:49:06 moko Exp $"

#include "pa_string.h"

struct Base64Options {
	bool strict;
	bool wrap;
	bool pad;
	const char *abc;

	Base64Options(bool awrap = true);
	void set_url_safe_abc();
};

size_t pa_base64_decode(const char *in, size_t in_size, char*& result, Base64Options options);
char* pa_base64_encode(const char *in, size_t in_size, Base64Options options = Base64Options());

#endif
