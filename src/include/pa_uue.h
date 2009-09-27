/** @file
	Parser: uue encoding module.

	Copyright (c) 2000-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

static const char * const IDENT_UUE_H="$Date: 2009/09/27 22:07:50 $";

#include "pa_string.h"
#include "pa_vfile.h"

const char* pa_uuencode(const String& file_name, const VFile& vfile);

#endif
