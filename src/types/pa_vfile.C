/** @file
	Parser: file object.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vfile.C,v 1.13 2001/04/09 11:30:44 paf Exp $
*/

#include "pa_config_includes.h"
#include <memory.h>
#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"

void VFile::set(bool tainted, 
				const void *avalue_ptr, size_t avalue_size,
				const char *afile_name,
				const String *mime_type) {
	fvalue_ptr=avalue_ptr;
	fvalue_size=avalue_size;

	ffields.clear();
	// $name
	if(afile_name) {
		char *lfile_name=(char *)malloc(strlen(afile_name)+1);
		strcpy(lfile_name, afile_name);
		if(char *after_slash=rsplit(lfile_name, '\\'))
			lfile_name=after_slash;
		if(char *after_slash=rsplit(lfile_name, '/'))
			lfile_name=after_slash;
		ffields.put(*name_name, NEW VString(*NEW String(pool(), lfile_name, 0, true)));
	}
	// $size
	ffields.put(*size_name, NEW VInt(pool(), fvalue_size));
	// $text
	String& text=*NEW String(pool());
	char *premature_zero_pos=(char *)memchr(fvalue_ptr, 0, fvalue_size);
	if(premature_zero_pos!=fvalue_ptr)
		text.APPEND((char *)fvalue_ptr, 
			premature_zero_pos?premature_zero_pos-(char *)fvalue_ptr:fvalue_size, 
			tainted? String::UL_TAINTED : String::UL_CLEAN,
			"user <input type=file>", 0);
	ffields.put(*text_name, NEW VString(text));
	// $mime-type
	if(mime_type)
		ffields.put(*vfile_mime_type_name, NEW VString(*mime_type));
}
