/** @file
	Parser: @b file parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_vfile.C,v 1.21 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_config_includes.h"

#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"

void VFile::set(bool tainted, 
				const void *avalue_ptr, size_t avalue_size,
				const char *afile_name,
				Value *mime_type) {
	fvalue_ptr=avalue_ptr;
	fvalue_size=avalue_size;

	const char *origin_file="user <input type=file>";

	ffields.clear();
	// $name
	char *lfile_name;
	if(afile_name) {
		lfile_name=(char *)malloc(strlen(afile_name)+1);
		strcpy(lfile_name, afile_name);
		if(char *after_slash=rsplit(lfile_name, '\\'))
			lfile_name=after_slash;
		if(char *after_slash=rsplit(lfile_name, '/'))
			lfile_name=after_slash;
		
	} else
		lfile_name=NONAME_DAT;
	String& sfile_name=*NEW String(pool());
	sfile_name.APPEND(lfile_name, 0, String::UL_FILE_SPEC, origin_file, 0);
	ffields.put(*name_name, NEW VString(sfile_name));
	// $size
	ffields.put(*size_name, NEW VInt(pool(), fvalue_size));
	// $text
	if(fvalue_ptr) { // assigned files don't have bytes
		String& text=*NEW String(pool());
		char *premature_zero_pos=(char *)memchr(fvalue_ptr, 0, fvalue_size);
		if(premature_zero_pos!=fvalue_ptr)
			text.APPEND((char *)fvalue_ptr, 
				premature_zero_pos?premature_zero_pos-(char *)fvalue_ptr:fvalue_size, 
				tainted? String::UL_TAINTED : String::UL_CLEAN,
				origin_file, 0);
		ffields.put(*text_name, NEW VString(text));
	}
	// $mime-type
	if(mime_type)
		ffields.put(*vfile_mime_type_name, mime_type);
}
