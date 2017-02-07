/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

#define IDENT_PA_UUE_H "$Id: pa_uue.h,v 1.13 2017/02/07 22:00:37 moko Exp $"

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name);

#endif
