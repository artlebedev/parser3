/*
  $Id: pa_vcframe.h,v 1.6 2001/03/10 11:44:32 paf Exp $
*/

#ifndef PA_VCFRAME_H
#define PA_VCFRAME_H

#include "pa_wwrapper.h"
#include "pa_vunknown.h"

class VCodeFrame : public WWrapper {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "code_frame"; }

	// codeframe: accumulated string
	const String *get_string() { return &string; };

public: // WContext

	// codeframe: intercepting string writes 
	void write(const String& astring, String::Untaint_lang lang);

public: // usage

	VCodeFrame(Pool& apool, WContext& awcontext) : 
		WWrapper(apool, &awcontext, awcontext.constructing()),

		string(apool) {
	}

private:
	
	String string;

};

#endif
