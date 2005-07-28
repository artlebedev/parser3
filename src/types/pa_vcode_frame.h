/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

static const char * const IDENT_VCODE_FRAME_H="$Date: 2005/07/28 11:23:02 $";

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
	override const Junction* put_element(Value& aself, const String& aname, Value* avalue, bool areplace) { 
		// $hash[^if(1){$.field[]}]
		// put goes to $hash
		return wcontext.put_element(aself, aname, avalue, areplace); 
	}

public: // WContext

	/// VCodeFrame: intercepting string writes 
	override void write(const String& string, String::Language lang) {
		fstring.append(string, lang);
	}

	/** VCodeFrame: twice transparent

		if value is VString writes fstring,
		else writes Value; raises an error if already

		$hash[^if(1){$hash}]
	*/
	override void write(Value& avalue) {
		if(const String* lstring=avalue.get_string())
			write(*lstring, String::L_PASS_APPENDED);
		else
			wcontext.write(avalue);
	}


public: // usage

	VCodeFrame(WContext& awcontext, WContext *parent): 
		WContext(&awcontext, parent),
		wcontext(awcontext) {}

private:

	WContext& wcontext;

};

#endif
