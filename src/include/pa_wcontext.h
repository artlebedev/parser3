/*
  $Id: pa_wcontext.h,v 1.13 2001/02/23 17:12:57 paf Exp $
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
	String *get_string() { return string; };

public: // usage

	WContext(Pool& apool, Value *avalue) : Value(apool), 
		fvalue(avalue),
		string(new(apool) String(apool)) {
	}

	// appends a string to result
	void write(String *astring) {
		if(!astring)
			return;

		*string+=*astring;
	}
	// if value is VString writes string,
	// else writes Value; raises an error if already
	void write(Value *avalue) {
		if(!avalue)
			return;

		String *string=avalue->get_string();
		if(string)
			write(string);
		else
			if(fvalue) // already have value?
				THROW(0,0,  // don't need to construct twice
					fvalue->name(),
					"value already assigned, use constructor to reassign it");
			else
				fvalue=avalue;
	}

	// retrives the resulting value
	// that can be VString if value==0 or the Value object
	Value *value() const {
		return fvalue?fvalue:NEW VString(string);
	}

protected:
	Value *fvalue;
private:
	String *string;
};

#endif
