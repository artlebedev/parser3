/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_UUE_H
#define PA_UUE_H

#define IDENT_PA_UUE_H "$Id: pa_uue.h,v 1.17 2026/04/25 13:38:46 moko Exp $"

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name);

#endif
