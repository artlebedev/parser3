/** @file
	Parser: form class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vform.h,v 1.14 2001/03/24 08:54:05 paf Exp $
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

#include "pa_common.h"
#include "pa_vstateless_class.h"
#include "_form.h"

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
	
	// all: for error reporting after fail(), etc
	const char *type() const { return "form"; }

	// form: CLASS,BASE,method,field
	Value *get_element(const String& aname) {
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(aname))
			return result;

		// $element
		return static_cast<Value *>(fields.get(aname));
	}

public: // usage

	VForm(Pool& apool) : VStateless_class(apool, form_base_class),
		fields(apool) {
	}

//	Hash& fields() { return ffields; }

	void fill_fields(Request& request, size_t post_max_size);

private:

	char *strpart(const char *str, size_t len);
	char *getAttributeValue(const char *data,char *attr,size_t len);
	void UnescapeChars(char **sp, const char *cp, size_t len);
	void ParseGetFormInput(const char *query_string);
	void ParsePostFormInput(const char *content_type, size_t post_size, bool mime_mode);
	void ParseFormInput(const char *data, size_t length);
	void ParseMimeInput(const char *content_type, const char *data, size_t length);
	void AppendFormEntry(
		const char *name, 
		const char *value_ptr, size_t value_size=0,
		const char *file_name=0);

private:

	Hash fields;

};

#endif
