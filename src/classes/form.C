/** @file
	Parser: @b form parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: form.C,v 1.19 2001/11/05 11:46:20 paf Exp $
*/

#include "classes.h"
#include "pa_request.h"
#include "pa_vform.h"

/// $LIMITS.max_post_size default 10M
const size_t MAX_POST_SIZE_DEFAULT=10*0x400*400;

// defines

#define FORM_CLASS_NAME "form"

#define LIMITS_NAME "LIMITS"

#define MAX_POST_SIZE_NAME "post_max_size"

// class

class MForm : public Methoded {
public:
	MForm(Pool& pool);
public: // Methoded
	bool used_directly() { return false; }
	void configure_admin(Request& r);
private:
	String max_post_size_name;
	String limits_name;
};

// methods

// constructor & configurator

MForm::MForm(Pool& apool) : Methoded(apool),
	max_post_size_name(apool, MAX_POST_SIZE_NAME),
	limits_name(apool, LIMITS_NAME)
{
	set_name(*NEW String(pool(), FORM_CLASS_NAME));
}

void MForm::configure_admin(Request& r) {
	Pool& pool=r.pool();

	Value *limits=r.main_class?r.main_class->get_element(limits_name):0;
	if(r.info.method && StrEqNc(r.info.method, "post", true)) {
		// $limits.max_post_size default 10M
		Value *element=limits?limits->get_element(max_post_size_name):0;
		size_t value=element?(size_t)element->as_double():0;
		size_t max_post_size=value?value:MAX_POST_SIZE_DEFAULT;
		
		if(r.info.content_length>max_post_size)
			throw Exception(0, 0,
				0,
				"posted content_length(%u) > max_post_size(%u)",
					r.post_size, max_post_size);
		if(r.info.content_length<0)
			throw Exception(0, 0,
				0,
				"posted content_length(%u) < 0",
					r.post_size);

		// read POST data
		if(r.info.content_length) {
			r.post_data=(char *)pool.malloc(r.info.content_length);
			r.post_size=SAPI::read_post(pool, r.post_data, r.info.content_length);
		}
		if(r.post_size!=r.info.content_length)
			throw Exception(0, 0, 
				0, 
				"post_size(%u)!=content_length(%u)", 
					r.post_size, r.info.content_length);
	}
}

// global variable

Methoded *form_base_class;

// creator

Methoded *MForm_create(Pool& pool) {
	return form_base_class=new(pool) MForm(pool);
}
