/** @file
	Parser: uue encoding module.

	Copyright (c) 2000,2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_UUE_H
#define PA_UUE_H

static const char * const IDENT_UUE_H="$Date: 2003/11/20 16:34:25 $";

#include "pa_string.h"
#include "pa_vfile.h"

void pa_uuencode(String& result, const String& file_name, const VFile& vfile);

#endif
