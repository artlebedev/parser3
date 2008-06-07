/** @file
	Parser: @b form class decls.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

static const char * const IDENT_VFORM_H="$Date: 2008/06/07 17:28:09 $";

// includes

#include "classes.h"
#include "pa_common.h"
#include "pa_value.h"

// defines

#define FORM_FIELDS_ELEMENT_NAME "fields"
#define FORM_TABLES_ELEMENT_NAME "tables"
#define FORM_IMAP_ELEMENT_NAME "imap"
#define FORM_FILES_ELEMENT_NAME "files"

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
	Charset* get_post_charset();

public: // usage

	VForm(Request_charsets& acharsets, Request_info& arequest_info);
	
private:

	Request_charsets& fcharsets;
	Request_info& frequest_info;

	char *strpart(const char* str, size_t len);
	char *getAttributeValue(const char* data,char *attr,size_t len);
	void UnescapeChars(char **sp, const char* cp, size_t len);
	void ParseGetFormInput(const char* query_string, size_t length);
	void ParseFormInput(const char* data, size_t length);
	void ParseMimeInput(char *content_type, const char* data, size_t length);
	void AppendFormEntry(
		const char* cname_cstr, 
		const char* raw_cvalue_ptr, const size_t raw_cvalue_size);
	void AppendFormFileEntry(
		const char* cname_cstr, 
		const char* raw_cvalue_ptr, const size_t raw_cvalue_size,
		const char* file_name_cstr);

	bool should_refill_fields_tables_and_files();
	void refill_fields_tables_and_files();

private:

	Charset* filled_source;
	Charset* filled_client;
	Charset* filled_post; // charset which was specified in content-type in incoming POST
	HashStringValue fields;
	HashStringValue tables;
	HashStringValue files;
	HashStringValue imap;

private:

	String::C transcode(const char* client, size_t client_size);

};

#endif
