/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

#define IDENT_PA_UUE_H "$Id: pa_uue.h,v 1.14 2020/12/15 17:10:32 moko Exp $"

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name);

#endif
