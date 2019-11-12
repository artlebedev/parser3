/** @file
	Parser: base64 functions.

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_BASE64_H
#define PA_BASE64_H

#define IDENT_PA_BASE64_H "$Id: pa_base64.h,v 1.1 2019/11/12 21:13:31 moko Exp $"

#include "pa_string.h"

void pa_base64_decode(const char *in, size_t in_size, char*& result, size_t& result_size, bool strict=false);
char* pa_base64_encode(const char *in, size_t in_size);
char* pa_base64_encode(const String& file_spec);

#endif
