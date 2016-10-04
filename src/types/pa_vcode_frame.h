/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

#define IDENT_PA_VCODE_FRAME_H "$Id: pa_vcode_frame.h,v 1.40 2016/10/04 21:10:15 moko Exp $"

#include "pa_wcontext.h"
#include "pa_vvoid.h"

/// specialized write wrapper, completely transparent
class VCodeFrame: public WContext {
public: // Value

	override const char* type() const { return "code_frame"; }

	/// VCodeFrame: twice transparent
	override const VJunction* put_element(const String& aname, Value* avalue) {
		// $hash[^if(1){$.field[]}]
		// put goes to $hash
		return fparent->put_element(aname, avalue); 
	}


public: // usage

	VCodeFrame(WContext& parent):
		WContext(&parent){
	}

};

#endif
