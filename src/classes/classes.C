/** @file
	Parser: @b Methoded and Methoded_array classes.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_request.h"

volatile const char * IDENT_CLASSES_C="$Id: classes.C,v 1.31 2020/12/15 17:10:27 moko Exp $" IDENT_CLASSES_H;

// Methoded

void Methoded::register_directly_used(Request& r) {
	if(used_directly())
		r.put_class(this);

	// prevent system classes from modification [calling set_method]
	// ^process[$string:CLASS]{@method} prohibited from now on...
	this->lock();
}

// Methoded_array

// global variable

Methoded_array& methoded_array() {
	static Methoded_array* result=0;
	if(!result)
		result=new Methoded_array;

	return *result;
}

// methods

Methoded_array::Methoded_array() {
#	include "classes.inc"
}

static void configure_admin_one(Methoded_array::element_type methoded, Request *r) {
	methoded->configure_admin(*r);
}
void Methoded_array::configure_admin(Request& r) {
	for_each(configure_admin_one, &r);
}

static void configure_user_one(Methoded_array::element_type methoded, Request *r) {
	methoded->configure_user(*r);
}
void Methoded_array::configure_user(Request& r) {
	for_each(configure_user_one, &r);
}

static void register_one(Methoded_array::element_type methoded, Request *r) {
	methoded->register_directly_used(*r);
}
void Methoded_array::register_directly_used(Request& r) {
	for_each(register_one, &r);
}
