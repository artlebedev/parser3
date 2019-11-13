/** @file
	Parser: base64 functions.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_BASE64_H
#define PA_BASE64_H

#define IDENT_PA_BASE64_H "$Id: pa_base64.h,v 1.2 2019/11/13 22:05:48 moko Exp $"

#include "pa_string.h"

struct Base64Options {
	bool strict;
	bool wrap;
	bool pad;
	const char *abc;

	Base64Options();
	void set_url_safe_abc();
};

void pa_base64_decode(const char *in, size_t in_size, char*& result, size_t& result_size, Base64Options options);
char* pa_base64_encode(const char *in, size_t in_size, Base64Options options = Base64Options());
char* pa_base64_encode(const String& file_spec, Base64Options options);

#endif
