/** @file
	Parser: @b form parser class.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vform.h"

volatile const char * IDENT_FORM_C="$Id: form.C,v 1.54 2026/10/16 15:16:21 moko Exp $";

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

// methods

// constructor & configurator

void MForm::configure_admin(Request& r) {

	if(r.request_info.can_have_body()){
		if(r.request_info.content_length>pa_post_max_size)
			throw Exception(PARSER_RUNTIME, 0, "posted content_length(%s) > $LIMITS.post_max_size(%s)", pa_uitoa(r.request_info.content_length), pa_uitoa(pa_post_max_size));

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
			throw Exception(0, 0, "post_size(%s) != content_length(%s)", pa_uitoa(r.request_info.post_size), pa_uitoa(r.request_info.content_length));
	}
}
