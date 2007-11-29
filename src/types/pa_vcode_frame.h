/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

static const char * const IDENT_VCODE_FRAME_H="$Date: 2007/11/29 08:06:12 $";

#include "pa_wcontext.h"
#include "pa_vvoid.h"

/// specialized write wrapper, completely transparent, but intercepting string writes
class VCodeFrame: public WContext {
public: // Value

	override const char* type() const { return "code_frame"; }
	/// VCodeFrame: twice transparent
	override Value* get_element(const String& aname, Value& aself, bool looking_up) { 
		return wcontext.get_element(aname, aself, looking_up); 
	}
	/// VCodeFrame: twice transparent
	override const VJunction* put_element(Value& aself, const String& aname, Value* avalue, bool areplace) { 
		// $hash[^if(1){$.field[]}]
		// put goes to $hash
		return wcontext.put_element(aself, aname, avalue, areplace); 
	}


public: // usage

	VCodeFrame(WContext& parent): 
		WContext(0, &parent),
		wcontext(parent) {}

private:

	WContext& wcontext;

};

#endif
