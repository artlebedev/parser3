/** @file
	Parser: @b form class decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

static const char* IDENT_VFORM_H="$Date: 2002/10/21 14:25:45 $";

#include "classes.h"
#include "pa_common.h"

#define FORM_FIELDS_ELEMENT_NAME "fields"
#define FORM_TABLES_ELEMENT_NAME "tables"
#define FORM_IMAP_ELEMENT_NAME "imap"

class Request;

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
class VForm : public VStateless_class {
public: // Value
	
	const char *type() const { return "form"; }
	
	// form: CLASS,method,field,tables field
	Value *get_element(const String& aname, Value *aself, bool /*looking_up*/);

public: // usage

	VForm(Pool& apool);
	
	void fill_fields_and_tables(Request& request);

private:

	char *strpart(const char *str, size_t len);
	char *getAttributeValue(const char *data,char *attr,size_t len);
	void UnescapeChars(char **sp, const char *cp, size_t len);
	void ParseGetFormInput(const char *query_string, size_t length);
	void ParseFormInput(const char *data, size_t length);
	void ParseMimeInput(char *content_type, const char *data, size_t length);
	void AppendFormEntry(
		const char *name, 
		const char *value_ptr, const size_t value_size,
		const char *file_name=0);
	void AppendFormEntry(const char *name, Value *value);

private:

	bool filled;
	Hash fields;
	Hash tables;
	Hash imap;

private:

	void transcode(
		const void *source_body, size_t source_content_length,
		const void *& dest_body, size_t& dest_content_length);

};

#endif
