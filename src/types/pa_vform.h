/** @file
	Parser: @b form class decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vform.h,v 1.35 2002/06/24 11:59:34 paf Exp $
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

#include "classes.h"
#include "pa_common.h"

#define FORM_FIELDS_ELEMENT_NAME "fields"
#define FORM_TABLES_ELEMENT_NAME "tables"

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
	Value *get_element(const String& aname);

public: // usage

	VForm(Pool& apool);
	
	void fill_fields_and_tables(Request& request);

private:

	char *strpart(const char *str, size_t len);
	char *getAttributeValue(char *data,char *attr,size_t len);
	void UnescapeChars(char **sp, const char *cp, size_t len);
	void ParseGetFormInput(char *query_string, size_t length);
	void ParseFormInput(char *data, size_t length);
	void ParseMimeInput(char *content_type, char *data, size_t length);
	void AppendFormEntry(
		const char *name, 
		char *value_ptr, size_t value_size,
		const char *file_name=0);

private:

	bool filled;
	Hash fields;
	Hash tables;

private:

	void transcode(
		const void *source_body, size_t source_content_length,
		const void *& dest_body, size_t& dest_content_length);

};

#endif
