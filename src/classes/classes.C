/** @file
	Parser: @b Methoded and Methoded_array classes.

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "classes.h"
#include "pa_request.h"

volatile const char * IDENT_CLASSES_C="$Id: classes.C,v 1.34 2024/11/11 05:28:00 moko Exp $" IDENT_CLASSES_H;

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
	ADD_CLASS_VAR(array)
	ADD_CLASS_VAR(bool)
	ADD_CLASS_VAR(curl)
	ADD_CLASS_VAR(date)
	ADD_CLASS_VAR(double)
	ADD_CLASS_VAR(file)
	ADD_CLASS_VAR(form)
	ADD_CLASS_VAR(hash)
	ADD_CLASS_VAR(hashfile)
	ADD_CLASS_VAR(image)
	ADD_CLASS_VAR(inet)
	ADD_CLASS_VAR(int)
	ADD_CLASS_VAR(json)
	ADD_CLASS_VAR(mail)
	ADD_CLASS_VAR(math)
	ADD_CLASS_VAR(memcached)
	ADD_CLASS_VAR(memory)
	ADD_CLASS_VAR(reflection)
	ADD_CLASS_VAR(regex)
	ADD_CLASS_VAR(response)
	ADD_CLASS_VAR(string)
	ADD_CLASS_VAR(table)
	ADD_CLASS_VAR(void)
	ADD_CLASS_VAR(xnode) // should be before xdoc
	ADD_CLASS_VAR(xdoc)
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
