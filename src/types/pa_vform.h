/** @file
	Parser: form class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vform.h,v 1.12 2001/03/19 23:07:24 paf Exp $
*/

#ifndef PA_VFORM_H
#define PA_VFORM_H

#include "pa_common.h"
#include "pa_vstateless_class.h"
#include "_form.h"

class Request;

/**
	derivates from VStateless_class so that :CLASS element referred to \a this.

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

	void fill_fields(Request& request, int post_max_size);

private:

	char *strpart(const char *str, int len);
	char *getAttributeValue(const char *data,char *attr,int len);
	void UnescapeChars(char **sp, const char *cp, int len);
	void ParseGetFormInput(const char *query_string);
	void ParsePostFormInput(const char *content_type, int post_size, bool mime_mode);
	void ParseFormInput(const char *data, int length);
	void ParseMimeInput(const char *content_type, const char *data, int length);
	void AppendFormEntry(
		const char *name, 
		const char *value_ptr, int value_size=0,
		const char *file_name=0);

private:

	Hash fields;

};

#endif
