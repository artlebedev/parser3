/** @file
	Parser: uue encoding module.

	Copyright (c) 2000,2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_uue.h,v 1.1 2002/06/24 11:59:32 paf Exp $
*/

#ifndef PA_UUE_H
#define PA_UUE_H

#include "pa_string.h"
#include "pa_vfile.h"

void pa_uuencode(String& result, const char *file_name_cstr, const VFile& vfile);

#endif
