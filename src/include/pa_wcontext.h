/*
  $Id: pa_wcontext.h,v 1.8 2001/02/22 15:39:22 paf Exp $
*/

/*
	data core
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#include "pa_value.h"
#include "pa_vstring.h"

class WContext : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "WContext"; }
	// wcontext: accumulated string
	String *get_string() { return string; };
	// wcontext: transparent
	Value *get_element(const String& name) { return check_value()->get_element(name); }
	// wcontext: transparent
	void put_element(const String& name, Value *avalue){ check_value()->put_element(name, avalue); }
	// wcontext: transparent
	Method *get_method(const String& name) const { return check_value()->get_method(name); }
	// wcontext: none yet | transparent
	VClass *get_class() const { return fvalue?fvalue->get_class():0; }
	// wcontext: none yet | transparent
	bool is_or_derived_from(VClass& ancestor) { return fvalue?fvalue->is_or_derived_from(ancestor):false; }

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
					0,
					"value already assigned, use constructor to reassign it");
			else
				fvalue=avalue;
	}

	// retrives the resulting value
	// that can be VString if value==0 or the Value object
	Value *value() const {
		return fvalue?fvalue:NEW VString(string);
	}
	
private:
	String *string;
	Value *fvalue;

private:
	// raises an exception on 0 value
	Value *check_value() const {
		if(!fvalue)
			THROW(0,0,
				0,
				"accessing wcontext without value");

		return fvalue;
	}
};

#endif
