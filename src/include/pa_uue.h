/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

static const char * const IDENT_UUE_H="$Date: 2009/09/28 08:59:12 $";

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name);

#endif
