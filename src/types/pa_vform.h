/** @file
	Parser: @b form class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vform.h,v 1.26 2001/10/24 14:39:44 parser Exp $
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

#include "classes.h"
#include "pa_common.h"

#define FORM_FIELDS_ELEMENT_NAME "fields"
#define FORM_TABLES_ELEMENT_NAME "tables"

extern Methoded *form_base_class;

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
	
	// form: CLASS,method,fields,tables,field
	Value *get_element(const String& aname);

public: // usage

	VForm(Pool& apool) : VStateless_class(apool, form_base_class),
		fields(apool),
		tables(apool) {
	}

	void fill_fields_and_tables(Request& request);

private:

	char *strpart(const char *str, size_t len);
	char *getAttributeValue(const char *data,char *attr,size_t len);
	void UnescapeChars(char **sp, const char *cp, size_t len);
	void ParseGetFormInput(char *query_string);
	void ParseFormInput(char *data, size_t length);
	void ParseMimeInput(const char *content_type, char *data, size_t length);
	void AppendFormEntry(
		const char *name, 
		char *value_ptr, size_t value_size=0,
		const char *file_name=0);

private:

	Hash fields;
	Hash tables;

};

#endif
