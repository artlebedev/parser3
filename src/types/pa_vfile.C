
/** @file
	Parser: @b file parser type.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_base64.h"
#include "pa_vfile.h"
#include "pa_vstring.h"
#include "pa_vint.h"
#include "pa_charsets.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VFILE_C="$Id: pa_vfile.C,v 1.80 2020/12/15 17:10:40 moko Exp $" IDENT_PA_VFILE_H;

// externs

extern Methoded* file_class;

// defines for statics

#define SIZE_NAME "size"
#define TEXT_NAME "text"

#define MODE_VALUE_TEXT "text"
#define MODE_VALUE_BINARY "binary"

#define CONTENT_TYPE_TEXT "text/plain"
#define CONTENT_TYPE_BINARY "application/octet-stream"

// statics

static const String size_name(SIZE_NAME);
static const String text_name(TEXT_NAME);

static const String mode_value_text(MODE_VALUE_TEXT);
static const String mode_value_binary(MODE_VALUE_BINARY);

static const String content_type_text(CONTENT_TYPE_TEXT);
static const String content_type_binary(CONTENT_TYPE_BINARY);

inline bool content_type_is_default(Value *content_type){
	if(content_type){
		const String *ct=content_type->get_string();
		return ct == &content_type_text || ct == &content_type_binary;
	}
	return true;
}

// methods

VStateless_class *VFile::get_class() { return file_class; }

HashStringValue *VFile::get_hash() { Value *prefetch PA_ATTR_UNUSED =get_element(text_name); return &ffields; }


void VFile::set_all(bool atainted, bool ais_text_mode, const char* avalue_ptr, size_t avalue_size, const String* afile_name) {
	fvalue_ptr=avalue_ptr;
	fvalue_size=avalue_size;

	ftext_tainted=atainted;
	fis_text_content=ais_text_mode;

	ffields.clear();

	set_name(afile_name);
	ffields.put(size_name, new VDouble(fvalue_size));
	set_mode(ais_text_mode);
}

void VFile::set(bool atainted, bool ais_text_mode, char* avalue_ptr, size_t avalue_size, const String* afile_name, Value* acontent_type, Request* r) {
	if(ais_text_mode && avalue_ptr && avalue_size) {
		fix_line_breaks(avalue_ptr, avalue_size);
	}
	set_all(atainted, ais_text_mode, avalue_ptr, avalue_size, afile_name);
	set_content_type(acontent_type, afile_name, r);
}

void VFile::set_binary(bool atainted, const char* avalue_ptr, size_t avalue_size, const String* afile_name, Value* acontent_type, Request* r) {
	set_all(atainted, false, avalue_ptr, avalue_size, afile_name);
	set_content_type(acontent_type, afile_name, r);
}

void VFile::set_binary_string(bool atainted, const char* avalue_ptr, size_t avalue_size) {
	set_all(atainted, false, avalue_ptr, avalue_size, 0);
}

void VFile::set(VFile& avfile, bool *ais_text_mode, const String* afile_name, Value* acontent_type, Request* r) {
	fvalue_ptr=avfile.fvalue_ptr;
	fvalue_size=avfile.fvalue_size;
	ftext_tainted=avfile.ftext_tainted;
	fis_text_mode=avfile.fis_text_mode;
	fis_text_content=avfile.fis_text_content;

	ffields.clear();
	for(HashStringValue::Iterator i(avfile.ffields); i; i.next())
		if(i.key() != text_name) // do not copy cached .text value
			ffields.put(*new String(i.key(), String::L_TAINTED), i.value());

	if(ais_text_mode)
		set_mode(*ais_text_mode);

	if(afile_name)
		set_name(afile_name);

	if(acontent_type || afile_name || ( ais_text_mode && content_type_is_default(ffields.get(content_type_name)) ))
		set_content_type(acontent_type, afile_name, r);
}

const char* VFile::text_cstr() {
	const char* p=value_ptr();
	if(fis_text_content)
		return p;

	size_t size=fvalue_size;

	if(const char *premature_zero_pos=(const char *)memchr(p, 0, size))
		size=premature_zero_pos-p;

	char *copy_ptr=size?pa_strdup(p, size):0;
	// text mode but binary content
	if(fis_text_mode && size)
		fix_line_breaks(copy_ptr, size);
	return copy_ptr;
}

void VFile::set_mode(bool ais_text_mode){
	fis_text_mode=ais_text_mode;
	if(fvalue_ptr)
		ffields.put(mode_name, new VString(ais_text_mode? mode_value_text : mode_value_binary ));
}

void VFile::set_name(const String* afile_name){
	const char *lfile_name;
	if(afile_name && !afile_name->is_empty()) {
		if(afile_name->starts_with("http://") || afile_name->starts_with("https://")){
			size_t query=afile_name->pos('?');
			if(query!=STRING_NOT_FOUND)
				afile_name=&afile_name->mid(0,query);
		}
		lfile_name=pa_filename(afile_name->taint_cstr(String::L_FILE_SPEC));
		if(!lfile_name[0])
			lfile_name=NONAME_DAT;
	} else
		lfile_name=NONAME_DAT;

	ffields.put(name_name, new VString(*new String(lfile_name, String::L_FILE_SPEC)));
}

void VFile::set_content_type(Value* acontent_type, const String* afile_name, Request* r){
	if(!acontent_type && afile_name && r)
		acontent_type=new VString(r->mime_type_of(afile_name));

	if(!acontent_type)
		acontent_type=new VString(fis_text_mode ? content_type_text : content_type_binary);

	ffields.put(content_type_name, acontent_type);
}

Charset* VFile::detect_binary_charset(Charset *charset){
	if(!charset)
		if(Value* content_type=ffields.get(content_type_name))
			if(const String *ct=content_type->get_string())
				charset=detect_charset(ct->cstr());
	return pa_charsets.checkBOM((char*&)fvalue_ptr, fvalue_size, charset); // checkBOM can alter ptr, but not the content
}

void VFile::transcode(Charset& from_charset, Charset& to_charset){
	String::C result=Charset::transcode(String::C(fvalue_ptr, fvalue_size), from_charset, to_charset);
	fvalue_ptr=result.str;
	fvalue_size=result.length;
	ffields.put(size_name, new VInt(fvalue_size));
}

void VFile::save(Request_charsets& charsets, const String& file_spec, bool is_text, Charset* asked_charset) {
	if(fvalue_ptr)
		file_write(charsets, file_spec, fvalue_ptr, fvalue_size, is_text, false/*do_append*/, asked_charset);
	else
		throw Exception(PARSER_RUNTIME, &file_spec, "saving stat-ed file");
}

bool VFile::is_text_mode(const String& mode) {
	if(mode==mode_value_text)
		return true;
	if(mode==mode_value_binary)
		return false;
	throw Exception(PARSER_RUNTIME, &mode, "is invalid mode, must be either '" MODE_VALUE_TEXT "' or '" MODE_VALUE_BINARY "'");
}

bool VFile::is_valid_mode(const String& mode) {
	return (mode==mode_value_text || mode==mode_value_binary);
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
					result.append_help_length(encoded, 0, String::L_JSON);
					result << "\"";
					break;
				}
			case Json_options::F_TEXT:
				{
					indent ? result << *indent : result << ",\n\"";
					result << "text\":\"";
					result.append_help_length(text_cstr(), 0, String::L_JSON);
					result << "\"";
					break;
				}
			case Json_options::F_BODYLESS: break;
		}
	}

	result << "\n" << options.indent << "}";
	return &result;
}
