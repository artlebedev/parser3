/** @file
	Parser: Value class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VALUE_C="$Date: 2002/08/29 12:22:48 $";

#include "pa_value.h"
#include "pa_vstateless_class.h"
#include "pa_vmethod_frame.h"

/// call this before invoking to ensure proper actual numbered params count
void Method::check_actual_numbered_params(
	Value& self, const String& actual_name, Array *actual_numbered_params) const {

	int actual_count=actual_numbered_params?actual_numbered_params->size():0;
	if(actual_count<min_numbered_params_count) // not proper count? bark
		throw Exception("parser.runtime",
			&actual_name,
			"native method of %s (%s) accepts minimum %d parameter(s) (%d present)", 
				self.get_class()->name_cstr(),
				self.type(),
				min_numbered_params_count,
				actual_count);

}

Junction::Junction(Pool& apool,
	Value& aself,
	VStateless_class *avclass, const Method *amethod,
	VMethodFrame *amethod_frame,
	Value *arcontext,
	WContext *awcontext,
	const Array *acode) : Pooled(apool),
	
	self(aself),
	vclass(avclass), method(amethod),
	method_frame(amethod_frame),
	rcontext(arcontext),
	wcontext(awcontext),
	code(acode) {
	if(method_frame)
		method_frame->register_junction(*this);
}

void Junction::invalidate() {
	method_frame=0;
	rcontext=0;
	wcontext=0;
}

/*
void Junction::change_context(Junction *source) {
	if(source) {
		method_frame=source->method_frame;
		rcontext=source->rcontext;
		wcontext=source->wcontext;
	} else {
		method_frame=rcontext=0;
		wcontext=0;
	}
}
*/

