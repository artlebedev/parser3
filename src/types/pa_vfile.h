/** @file
	Parser: @b file parser type decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFILE_H
#define PA_VFILE_H

#define IDENT_PA_VFILE_H "$Id: pa_vfile.h,v 1.85 2020/12/15 17:10:40 moko Exp $"

// include

#include "pa_common.h"
#include "pa_globals.h"
#include "pa_vstateless_object.h"
#include "pa_vbool.h"

// defines

#define NONAME_DAT "noname.dat"
#define VFILE_TYPE "file"

#define MODE_NAME "mode"
static const String mode_name(MODE_NAME);

// forwards

class Methoded;

/** holds received from user or read from disk file.

	@see VForm
*/
class VFile: public VStateless_object {

	const char* fvalue_ptr;
	size_t fvalue_size;
	bool ftext_tainted;
	bool fis_text_mode;
	bool fis_text_content;
	HashStringValue ffields;

public: // Value
	
	override const char* type() const { return VFILE_TYPE; }
	override VStateless_class *get_class();

	/// VFile: true
	override bool as_bool() const { return true; }

	/// VFile: true
	override Value& as_expr_result() { return VBool::get(true); }

	/// VFile: this
	override VFile* as_vfile(String::Language /*lang*/, 
		const Request_charsets* /*charsets*/) { return this; }

	/// VFile: json-string
	override const String* get_json_string(Json_options& options);

	/// VFile: method,field
	override Value* get_element(const String& aname);

	/// VFile: fields
	override HashStringValue *get_hash();

public: // usage

	VFile(): fvalue_ptr(0), fvalue_size(0), ftext_tainted(false), fis_text_mode(false), fis_text_content(false){}

	VFile(HashStringValue& afields): fvalue_ptr(0), fvalue_size(0), ftext_tainted(false), fis_text_mode(false), fis_text_content(false), ffields(afields) {}

	/// WARNING: when setting text files be sure to append terminating zero to avalue_ptr
	/// WARNING: the content can be modified while creating "text" vfile
	void set(bool atainted, bool ais_text_mode, char* avalue_ptr, size_t avalue_size, const String* afile_name=0, Value* acontent_type=0, Request* r=0);
	void set(VFile& avfile, bool *ais_text_mode, const String* afile_name=0, Value* acontent_type=0, Request* r=0);
	void set_binary(bool atainted, const char* avalue_ptr, size_t avalue_size, const String* afile_name=0, Value* acontent_type=0, Request* r=0);
	void set_binary_string(bool atainted, const char* avalue_ptr, size_t avalue_size);

	void save(Request_charsets& charsets, const String& file_spec, bool is_text, Charset* asked_charset=0);

	static bool is_text_mode(const String& mode);
	static bool is_valid_mode (const String& mode);

	const char* value_ptr() const { 
		if(!fvalue_ptr)
			throw Exception(PARSER_RUNTIME, 0, "getting value of stat-ed file");
		return fvalue_ptr; 
	}

	size_t value_size() const { return fvalue_size; }
	bool is_text_mode() const { return fis_text_mode; }

	HashStringValue& fields() { return ffields; }

	Charset* detect_binary_charset(Charset* charset);
	void transcode(Charset& from_charset, Charset& to_charset);

private:
	const char* text_cstr();

	void set_all(bool atainted, bool ais_text_mode, const char* avalue_ptr, size_t avalue_size, const String* afile_name);

	void set_mode(bool ais_text);

	void set_name(const String* afile_name);

	void set_content_type(Value* acontent_type, const String* afile_name=0, Request* r=0);

};

#endif
