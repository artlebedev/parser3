/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vcframe.h,v 1.3 2001/03/16 11:10:21 paf Exp $
*/

#ifndef PA_VCFRAME_H
#define PA_VCFRAME_H

#include "pa_wwrapper.h"
#include "pa_vunknown.h"

class VCodeFrame : public WWrapper {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "code_frame"; }

public: // WContext

	// codeframe: intercepting string writes 
	void write(const String& astring, String::Untaint_lang lang) {
		fstring.append(astring, lang);
	}

public: // usage

	VCodeFrame(Pool& apool, WContext& awcontext) : 
		WWrapper(apool, &awcontext, awcontext.constructing()) {
	}

};

#endif
