/** @file
	Parser: @b form parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: form.C,v 1.4 2001/04/28 08:43:47 paf Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vform.h"

// defines

#define FORM_CLASS_NAME "form"

// class

class MForm : public Methoded {
public:
	MForm(Pool& pool);
	bool used_directly() { return false; }
};

// methods

// constructor

MForm::MForm(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), FORM_CLASS_NAME));
}

// global variable

Methoded *form_base_class;

// creator

Methoded *MForm_create(Pool& pool) {
	return form_base_class=new(pool) MForm(pool);
}
