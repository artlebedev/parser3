/** @file
	Parser: @b form class decls.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

static const char * const IDENT_VFORM_H="$Date: 2003/11/20 16:34:29 $";

// includes

#include "classes.h"
#include "pa_common.h"
#include "pa_value.h"

// defines

#define FORM_FIELDS_ELEMENT_NAME "fields"
#define FORM_TABLES_ELEMENT_NAME "tables"
#define FORM_IMAP_ELEMENT_NAME "imap"

// forwards

class Request_info;
class Request_charsets;

/**
	derivates from VStateless_class so that :CLASS element referred to @a this.

	and users could do such tricks:
	@verbatim
		^rem{pass somebody something with elements}

		^rem{this time that would be elements of a form}
		^somebody[$form:CLASS]

		^rem{this time that would be elements of a table record}
		$news[^table:sql[select * from news]]
		^somebody[^news.record[]]
	@endverbatim
*/
class VForm: public VStateless_class {
public: // Value
	
	const char* type() const { return "form"; }
	
	// form: CLASS,method,field,tables field
	Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // usage

	VForm();
	
	void fill_fields_and_tables(Request_charsets& acharsets, Request_info& request_info);

private:

	Request_charsets* fcharsets;

	char *strpart(const char* str, size_t len);
	char *getAttributeValue(const char* data,char *attr,size_t len);
	void UnescapeChars(char **sp, const char* cp, size_t len);
	void ParseGetFormInput(const char* query_string, size_t length);
	void ParseFormInput(const char* data, size_t length);
	void ParseMimeInput(char *content_type, const char* data, size_t length);
	void AppendFormEntry(
		const char* cname_cstr, 
		const char* raw_cvalue_ptr, const size_t raw_cvalue_size,
		const char* copy_me_file_name_cstr=0);

private:

	bool filled;
	HashStringValue fields;
	HashStringValue tables;
	HashStringValue imap;

private:

	String::C transcode(const char* client, size_t client_size);

};

#endif
