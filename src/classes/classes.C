/** @file
	Parser: @b Methoded and Methoded_array classes.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: classes.C,v 1.2 2001/04/28 08:45:15 paf Exp $
*/

#include "classes.h"
#include "pa_request.h"

// Methoded

void Methoded::register_directly_used(Request& r) {
	if(used_directly())
		r.classes().put(name(), this);
}

// Methoded_array

// global variable

Methoded_array *methoded_array;

// methods

Methoded_array::Methoded_array(Pool& apool) : Array(apool) {
#	include "classes.inc"
}

static void configure_one(Array::Item *value, void *info) {
	Request& r=*static_cast<Request *>(info);
	Methoded *methoded=static_cast<Methoded *>(value);
	methoded->configure(r);
}

void Methoded_array::configure(Request& r) {
	for_each(configure_one, &r);
}

static void register_one(Array::Item *value, void *info) {
	Request& r=*static_cast<Request *>(info);
	Methoded *methoded=static_cast<Methoded *>(value);
	methoded->register_directly_used(r);
}

void Methoded_array::register_directly_used(Request& r) {
	for_each(register_one, &r);
}

// creator

void init_methoded_array(Pool& pool) {
	methoded_array=new(pool) Methoded_array(pool);
}
