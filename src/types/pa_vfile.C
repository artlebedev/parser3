
/** @file
	Parser: @b file parser type.

	Copyright(c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VFILE_C="$Date: 2003/01/21 15:51:18 $";

#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"

void VFile::set(bool tainted, 
				const void *avalue_ptr, size_t avalue_size,
				const char *afile_name,
				Value *content_type) {
	fvalue_ptr=avalue_ptr;
	fvalue_size=avalue_size;

	const char *origin_file=afile_name?afile_name:"user (input type=file)";

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
	if(fvalue_ptr && fvalue_size) { // assigned files don't have ptr, and we really have some bytes
		String& text=*NEW String(pool());
		char *premature_zero_pos=(char *)memchr(fvalue_ptr, 0, fvalue_size);
		if(premature_zero_pos!=fvalue_ptr) {
			size_t copy_size=premature_zero_pos?premature_zero_pos-(char *)fvalue_ptr:fvalue_size;
			char *copy_ptr=(char *)malloc(copy_size);
			memcpy(copy_ptr, fvalue_ptr, copy_size);
			fix_line_breaks(copy_ptr, copy_size);
			text.APPEND(copy_ptr,  copy_size, 
				tainted? String::UL_TAINTED : String::UL_AS_IS,
				origin_file, 0);
		}
		ffields.put(*text_name, NEW VString(text));
	}
	// $mime-type
	if(content_type)
		ffields.put(*content_type_name, content_type);
}

Value *VFile::get_element(const String& aname, Value& aself, bool looking_up) {
	// $method
	if(Value *result=VStateless_object::get_element(aname, aself, looking_up))
		return result;

	// $field
	return static_cast<Value *>(ffields.get(aname));
}
