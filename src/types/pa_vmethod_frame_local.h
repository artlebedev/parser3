/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMETHOD_FRAME_LOCAL_H
#define PA_VMETHOD_FRAME_LOCAL_H

static const char * const IDENT_VMETHOD_FRAME_LOCAL_H="$Date: 2008/05/30 12:24:13 $";

#include "pa_vmethod_frame.h"


class VMethodFrameLocal: public VMethodFrame {

public:
	override const VJunction* put_element(Value& /*aself*/, const String& aname, Value* avalue, bool /*areplace*/) {
//		if(!my->get(aname))
			set_my_variable(aname, *avalue);

//		my->put_replaced(aname, avalue);
		return PUT_ELEMENT_REPLACED_ELEMENT;
	}

public:
	VMethodFrameLocal(
		const Junction& ajunction/*info: always method-junction*/,
		VMethodFrame *acaller
	): 
		VMethodFrame(ajunction, acaller){
	};

};

#endif
