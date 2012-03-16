/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

#define IDENT_PA_UUE_H "$Id: pa_uue.h,v 1.11 2012/03/16 09:24:11 moko Exp $"

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name);

#endif
