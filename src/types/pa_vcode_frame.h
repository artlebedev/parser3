/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vcode_frame.h,v 1.9 2001/10/13 15:59:43 parser Exp $
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
	void put_element(const String& name, Value *value){ wcontext.put_element(name, value); }

public: // WContext

	/// VCodeFrame: intercepting string writes 
	virtual void write(const String& string, String::Untaint_lang lang) {
		fstring.append(string, lang);
	}

	/// VCodeFrame: twice transparent
	virtual void write(Value& value) {
		// ^method[^if(){$hash}
		wcontext.write(value);
	}

	/// VCodeFrame: twice transparent
	virtual void write(Value& value, String::Untaint_lang lang) {
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
