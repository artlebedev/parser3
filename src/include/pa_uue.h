/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_UUE_H
#define PA_UUE_H

#define IDENT_PA_UUE_H "$Id: pa_uue.h,v 1.16 2024/11/04 03:53:25 moko Exp $"

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name);

#endif
