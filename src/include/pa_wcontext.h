/*
  $Id: pa_wcontext.h,v 1.19 2001/02/25 17:33:43 paf Exp $
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#include "pa_value.h"
#include "pa_vstring.h"
#include "pa_vhash.h"

class WContext : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "WContext"; }
	// WContext: accumulated string
	String *get_string() { return &string; };

	// WContext: none yet | transparent
	VClass *get_class() { return fvalue?fvalue->get_class():0; }
	// wcontext: transparent
	Value *get_aliased() { return fvalue?fvalue->get_aliased():0; }

public: // WContext

	// appends a string to result
	virtual void write(String *astring);

	// if value is VString writes string,
	// else writes Value; raises an error if already
	virtual void write(Value *avalue);

public: // usage

	WContext(Pool& apool, Value *avalue, bool aconstructing) : Value(apool), 
		fvalue(avalue),
		fconstructing(aconstructing),
		string(*new(apool) String(apool)) {
	}

	bool constructing() { return fconstructing; }

	// retrives the resulting value
	Value *object() const {
		return fvalue;
	}
	// retrives the resulting value
	// that can be VString if value==0 or the Value object
	Value *result() const {
		return fvalue?fvalue:NEW VString(string);
	}

protected:
	Value *fvalue;
private:
	bool fconstructing;
	String& string;
};

#endif
