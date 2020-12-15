/** @file
	Parser: @b form parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vform.h"

volatile const char * IDENT_FORM_C="$Id: form.C,v 1.50 2020/12/15 17:10:28 moko Exp $";

/// $LIMITS.max_post_size default 10M
const size_t MAX_POST_SIZE_DEFAULT=10*0x400*0x400;

// class

class MForm : public Methoded {
public: // Methoded

	bool used_directly() { return false; }
	void configure_admin(Request& r);

public:

	MForm(): Methoded("form") {}

};

// global variable

DECLARE_CLASS_VAR(form, new MForm);

// defines for statics

#define LIMITS_NAME "LIMITS"
#define MAX_POST_SIZE_NAME "post_max_size"

// statics

static const String max_post_size_name(MAX_POST_SIZE_NAME);
static const String limits_name(LIMITS_NAME);

// methods

// constructor & configurator

void MForm::configure_admin(Request& r) {

	Value* limits=r.main_class.get_element(limits_name);
	if(r.request_info.can_have_body()){
		// $limits.max_post_size default 10M
		Value* element=limits ? limits->get_element(max_post_size_name) : 0;
		size_t value=element ? (size_t)element->as_double() : 0;
		size_t max_post_size=value ? value : MAX_POST_SIZE_DEFAULT;
		
		if(r.request_info.content_length>max_post_size)
			throw Exception(PARSER_RUNTIME, 0, "posted content_length(%u) > $" LIMITS_NAME "." MAX_POST_SIZE_NAME "(%u)", r.request_info.content_length, max_post_size);

		// read POST data
		if(r.request_info.content_length) {
			char *post_data=new(PointerFreeGC) char[r.request_info.content_length+1/*terminating zero*/];
			size_t post_size=SAPI::read_post(r.sapi_info, post_data, r.request_info.content_length);
			post_data[post_size]=0; // terminating zero
			r.request_info.post_data=post_data;
			r.request_info.post_size=post_size;
		} else {
			r.request_info.post_data=0;
			r.request_info.post_size=0;
		}
		if(r.request_info.post_size!=r.request_info.content_length)
			throw Exception(0, 0, "post_size(%u) != content_length(%u)", r.request_info.post_size, r.request_info.content_length);
	}
}
