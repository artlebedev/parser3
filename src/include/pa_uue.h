/** @file
	Parser: uue encoding module.

	Copyright (c) 2000,2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

static const char* IDENT_UUE_H="$Date: 2002/08/01 11:41:16 $";

#include "pa_string.h"
#include "pa_vfile.h"

void pa_uuencode(String& result, const char *file_name_cstr, const VFile& vfile);

#endif
