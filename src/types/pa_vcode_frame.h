/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vcode_frame.h,v 1.13 2002/02/08 07:27:51 paf Exp $
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

#include "pa_wcontext.h"
#include "pa_vvoid.h"

/// specialized write wrapper, completely transparent, but intercepting string writes
class VCodeFrame : public WContext {
public: // Value

	const char *type() const { return "code_frame"; }
	/// VCodeFrame: twice transparent
	Value *get_element(const String& name) { return wcontext.get_element(name); }
	/// VCodeFrame: twice transparent
	void put_element(const String& name, Value *value){ 
		// $hash[^if(1){$.field[]}]
		// put goes to $hash
		wcontext.put_element(name, value); 
	}

public: // WContext

	/// VCodeFrame: intercepting string writes 
	virtual void write(const String& string, String::Untaint_lang lang) {
		fstring.append(string, lang);
	}

	/// VCodeFrame: twice transparent
	virtual void write(Value& value) {
		// ^method[^if(1){$hash}]
		// write goes ^method[here]
		wcontext.write(value);
	}

	/** VCodeFrame: twice transparent

		if value is VString writes fstring,
		else writes Value; raises an error if already
	*/
	virtual void write(Value& value, String::Untaint_lang lang) {
		if(const String *fstring=value.get_string())
			write(*fstring, lang);
		else
			wcontext.write(value, lang);		
	}

public: // usage

	VCodeFrame(Pool& apool, WContext& awcontext) : 
		WContext(apool, &awcontext),
		wcontext(awcontext) {
	}

private:

	WContext& wcontext;

};

#endif
