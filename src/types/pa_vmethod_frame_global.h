/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMETHOD_FRAME_GLOBAL_H
#define PA_VMETHOD_FRAME_GLOBAL_H

static const char * const IDENT_VMETHOD_FRAME_GLOBAL_H="$Date: 2012/03/16 09:24:18 $";

#include "pa_vmethod_frame.h"


class VMethodFrameGlobal: public VMethodFrame {

public:
	override const VJunction* put_element(Value& /*aself*/, const String& aname, Value* avalue, bool /*areplace*/) { 
		if(my && my->put_replaced(aname, avalue))
			return PUT_ELEMENT_REPLACED_ELEMENT;

		return self().put_element(self(), aname, avalue, false/*=always, areplace*/);
	}

public:
	VMethodFrameGlobal(
		const Junction& ajunction/*info: always method-junction*/,
		VMethodFrame *acaller
	): 
		VMethodFrame(ajunction, acaller){
	};
};

#endif
