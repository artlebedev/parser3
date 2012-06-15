
/** @file
	Parser: @b file parser type.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VFILE_C="$Id: pa_vfile.C,v 1.53 2012/06/15 06:13:11 misha Exp $" IDENT_PA_VFILE_H;

// externs

extern Methoded* file_class;

// defines for statics

#define SIZE_NAME "size"
#define TEXT_NAME "text"

#define MODE_VALUE_TEXT "text"
#define MODE_VALUE_BINARY "binary"

// statics

static const String size_name(SIZE_NAME);
static const String text_name(TEXT_NAME);

static const String mode_value_text(MODE_VALUE_TEXT);
static const String mode_value_binary(MODE_VALUE_BINARY);

// methods

VStateless_class *VFile::get_class() { return file_class; }

void VFile::set(
		bool atainted, 
		bool ais_text_mode,
		char* avalue_ptr,
		size_t avalue_size,
		const String* afile_name,
		Value* acontent_type,
		Request* r) {

	fvalue_ptr=avalue_ptr;
	fvalue_size=avalue_size;

	ftext_tainted=atainted;
	feols_normalized=false;

	ffields.clear();

	// $.size
	ffields.put(size_name, new VInt(fvalue_size));

	// $.mode
	set_mode(ais_text_mode);

	// $.name
	set_name(afile_name);

	// $mime-type
	set_content_type(acontent_type, afile_name, r);

	fix_line_breaks_set();
}

void VFile::set(VFile& avfile) {
	fvalue_ptr=avfile.fvalue_ptr;
	fvalue_size=avfile.fvalue_size;

	ftext_tainted=avfile.ftext_tainted;
	fis_text_mode=avfile.fis_text_mode;
	feols_normalized=avfile.feols_normalized;

	fix_line_breaks_set();

	ffields.clear();

	for(HashStringValue::Iterator i(avfile.ffields); i; i.next())
		if(i.key() != text_name) // do not copy cached .text value
			ffields.put_dont_replace(*new String(i.key(), String::L_TAINTED), i.value());
}

const char* VFile::text_cstr() {
	const char* p=value_ptr();
	if(feols_normalized)
		return p;

	size_t size=fvalue_size;

	// have to check for "text" files as well because mode could be changed after creation by set_mode()
	if(const char *premature_zero_pos=(const char *)memchr(p, 0, size))
		size=premature_zero_pos-p;

	char *copy_ptr=size?strdup(p, size):0;
	if(fis_text_mode && size)
		fix_line_breaks(copy_ptr, size);

	return copy_ptr;
}

void VFile::set_mode(bool ais_text_mode){
	fis_text_mode=ais_text_mode;
	ffields.put(mode_name, new VString(ais_text_mode? mode_value_text : mode_value_binary ));
}

void VFile::set_name(const String* afile_name){
	char *lfile_name;
	if(afile_name) {
		lfile_name=strdup(afile_name->taint_cstr(String::L_FILE_SPEC));
		if(char *after_slash=rsplit(lfile_name, '\\'))
			lfile_name=after_slash;
		if(char *after_slash=rsplit(lfile_name, '/'))
			lfile_name=after_slash;
	} else
		lfile_name=NONAME_DAT;

	ffields.put(name_name, new VString(*new String(lfile_name, String::L_FILE_SPEC)));
}

void VFile::set_content_type(Value* acontent_type, const String* afile_name, Request* r){
	if(!acontent_type && afile_name && r)
		acontent_type=new VString(r->mime_type_of(afile_name));

	if(acontent_type)
		ffields.put(content_type_name, acontent_type);
}

void VFile::save(Request_charsets& charsets, const String& file_spec, bool is_text, Charset* asked_charset) {
	if(fvalue_ptr)
		file_write(charsets, file_spec, fvalue_ptr, fvalue_size, is_text, false/*do_append*/, asked_charset);
	else
		throw Exception(PARSER_RUNTIME,
			&file_spec,
			"saving stat-ed file");
}

bool VFile::is_text_mode(const String& mode) {
	if(mode==mode_value_text)
		return true;
	if(mode==mode_value_binary)
		return false;
	throw Exception(PARSER_RUNTIME,
		&mode,
		"is invalid mode, must be either '"MODE_VALUE_TEXT"' or '"MODE_VALUE_BINARY"'");
}
	
bool VFile::is_valid_mode (const String& mode) {
	return (mode==mode_value_text || mode==mode_value_binary);
}

void VFile::fix_line_breaks_set() {
	if(!feols_normalized && fis_text_mode && fvalue_ptr && fvalue_size) {
		fix_line_breaks(fvalue_ptr, fvalue_size);
		feols_normalized=true;
	}
}

Value* VFile::get_element(const String& aname) {
	Value* result;

	// $method
	if(result=VStateless_object::get_element(aname))
		return result;

	// $field
	if(result=ffields.get(aname))
		return result;

	// $text - if not cached
	if(aname == text_name && fvalue_ptr && fvalue_size) {
		// assigned file have ptr and we really have some bytes

		result=new VString(*new String(text_cstr(), ftext_tainted ? String::L_TAINTED : String::L_AS_IS));

		// cache it
		ffields.put(text_name, result);

		return result;
	}

	return 0;
}

const String* VFile::get_json_string(Json_options& options){
	String& result=*new String("{\n", String::L_AS_IS);
	
	String * indent=NULL;

	if (options.indent){
		indent = new String(",\n\t", String::L_AS_IS); *indent << options.indent << "\"";
		result << "\t" << options.indent;
	}

	result << "\"class\":\"file\"";

	for(HashStringValue::Iterator i(ffields); i; i.next() ){
		String::Body key=i.key();
		if(key != text_name){
			indent ? result << *indent : result << ",\n\"";
			result << String(key, String::L_JSON) << "\":" << *i.value()->get_json_string(options);
		}
	}

	if(fvalue_ptr){
		switch(options.file){
			case Json_options::F_BASE64:
				{
					indent ? result << *indent : result << ",\n\"";
					result << "base64\":\"";
					const char* encoded=pa_base64_encode(fvalue_ptr, fvalue_size);
					result.append_help_length(encoded, strlen(encoded), String::L_JSON);
					result << "\"";
					break;
				}
			case Json_options::F_TEXT:
				{
					indent ? result << *indent : result << ",\n\"";
					result << "text\":\"";
					result.append(get_element(text_name)->as_string(), String::L_JSON, true/*forced lang*/);
					result << "\"";
					break;
				}
		}
	}

	result << "\n" << options.indent << "}";
	return &result;
}
