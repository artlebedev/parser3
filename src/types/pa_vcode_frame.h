/**	@file
	Parser: @b code_frame write wrapper write context

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vcode_frame.h,v 1.4 2001/05/07 14:00:53 paf Exp $
*/

#ifndef PA_VCODE_FRAME_H
#define PA_VCODE_FRAME_H

#include "pa_wwrapper.h"
#include "pa_vunknown.h"

/// specialized write wrapper, intercepting string writes
class VCodeFrame : public WWrapper {
public: // Value

	const char *type() const { return "code_frame"; }

public: // WContext

	/// VCodeFrame: intercepting string writes 
	void write(const String& astring, String::Untaint_lang lang) {
		fstring.append(astring, lang);
	}

public: // usage

	VCodeFrame(Pool& apool, WContext& awcontext) : 
		WWrapper(apool, &awcontext, awcontext.constructing()) {
	}

};

#endif
