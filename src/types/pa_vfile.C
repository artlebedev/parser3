
/** @file
	Parser: @b file parser type.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VFILE_C="$Date: 2005/08/05 13:03:04 $";

#include "classes.h"
#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"

// externs

extern Methoded* file_class;

// defines for statics

#define SIZE_NAME "size"
#define TEXT_NAME "text"

// statics

static const String size_name(SIZE_NAME);
static const String text_name(TEXT_NAME);

// methods

VStateless_class *VFile::get_class() { return file_class; }

void VFile::set(
		bool tainted, 
		const char* avalue_ptr, size_t avalue_size,
		const char* afile_name_cstr,
		Value* acontent_type) {
	fvalue_ptr=avalue_ptr;
	fvalue_size=avalue_size;

	ffields.clear();
	// $name
	char *lfile_name;
	if(afile_name_cstr) {
		lfile_name=strdup(afile_name_cstr);
		if(char *after_slash=rsplit(lfile_name, '\\'))
			lfile_name=after_slash;
		if(char *after_slash=rsplit(lfile_name, '/'))
			lfile_name=after_slash;
	} else
		lfile_name=NONAME_DAT;
	String& sfile_name=*new String;
	sfile_name.append_help_length(lfile_name, 0, String::L_FILE_SPEC);
	ffields.put(name_name, new VString(sfile_name));
	// $size
	ffields.put(size_name, new VInt(fvalue_size));
	// $text
	if(fvalue_ptr && fvalue_size) { // assigned files don't have ptr, and we really have some bytes
		String& text=*new String;
		char *premature_zero_pos=(char *)memchr(fvalue_ptr, 0, fvalue_size);
		if(premature_zero_pos!=fvalue_ptr) {
			size_t copy_size=premature_zero_pos?premature_zero_pos-fvalue_ptr:fvalue_size;
			char *copy_ptr=strdup(fvalue_ptr, copy_size);
			fix_line_breaks(copy_ptr, copy_size);
			text.append_know_length(copy_ptr, copy_size, tainted? String::L_TAINTED : String::L_AS_IS);
		}
		ffields.put(text_name, new VString(text));
	}
	// $mime-type
	if(acontent_type)
		ffields.put(content_type_name, acontent_type);
}

Value* VFile::get_element(const String& aname, Value& aself, bool looking_up) {
	// $method
	if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
		return result;

	// $field
	return ffields.get(aname);
}
