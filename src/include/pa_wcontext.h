/*
  $Id: pa_wcontext.h,v 1.27 2001/03/10 12:12:50 paf Exp $
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

class Request;

class WContext : public Value {
	friend Request;
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "wcontext"; }
	// WContext: accumulated fstring
	const String *get_string() { return &fstring; };

	// WContext: none yet | transparent
	VClass *get_class() { return fvalue?fvalue->get_class():0; }
	// wcontext: transparent
	VAliased *get_aliased() { return fvalue?fvalue->get_aliased():0; }

public: // WContext

	// appends a fstring to result
	virtual void write(const String& astring, String::Untaint_lang lang);

	// if value is VString writes fstring,
	// else writes Value; raises an error if already
	virtual void write(Value& avalue, String::Untaint_lang lang);

public: // usage

	WContext(Pool& apool, Value *avalue, bool aconstructing) : Value(apool), 
		fvalue(avalue),
		fconstructing(aconstructing),
		fstring(*new(apool) String(apool)) {
	}

	bool constructing() { return fconstructing; }

	// retrives the resulting value
	// that can be VString if value==0 or the Value object
	Value *result() const {
		return fvalue?fvalue:NEW VString(fstring);
	}

protected:
	Value *fvalue;
private:
	bool fconstructing;
	String& fstring;
};

#endif
