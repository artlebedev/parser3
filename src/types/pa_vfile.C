/**
	Parser: file class.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vfile.C,v 1.3 2001/03/19 20:47:22 paf Exp $
*/

#include <string.h>
#include <memory.h>
#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"

VFile::VFile(Pool& apool, 
			 const char *avalue_ptr, size_t avalue_size,
			 const char *afile_name) : VStateless_class(apool, file_base_class),
			 
			 fvalue_ptr(avalue_ptr),
			 fvalue_size(avalue_size),
			 fields(apool) {
	// $name
	char *lfile_name=(char *)malloc(strlen(afile_name)+1);
	strcpy(lfile_name, afile_name);
	if(char *after_slash=rsplit(lfile_name, '\\'))
		lfile_name=after_slash;
	if(char *after_slash=rsplit(lfile_name, '/'))
		lfile_name=after_slash;
	fields.put(*name_name, NEW VString(*NEW String(pool(), lfile_name, true)));
	// $size
	fields.put(*size_name, NEW VInt(pool(), fvalue_size));
	// $text
	String& text=*NEW String(pool());
	char *premature_zero_pos=(char *)memchr(fvalue_ptr, 0, fvalue_size);
	if(premature_zero_pos!=fvalue_ptr)
		text.APPEND(fvalue_ptr, 
			premature_zero_pos?premature_zero_pos-fvalue_ptr:fvalue_size, 
			"<input type=file>", 0);
	fields.put(*text_name, NEW VString(text));
}
