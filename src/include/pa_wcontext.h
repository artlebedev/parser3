/*
  $Id: pa_wcontext.h,v 1.2 2001/02/21 17:36:30 paf Exp $
*/

/*
	data core
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#include "pa_value.h"

class WContext : public Value {
public: // Value

	// all: for error reporting after fail(), etc
	/*virtual*/ const char *get_type() const { return "WContext"; }
	// wcontext: accumulated string
	/*virtual*/ String *get_string() { return &string; };
	// wcontext: transparent
	/*virtual*/ Value *get_element(const String& name) const { return check_value()->get_element(name); }
	// wcontext: transparent
	/*virtual*/ void put_element(const String& name, Value *avalue){ check_value()->put_element(name, avalue); }
	// wcontext: transparent
	/*virtual*/ Method *get_method(const String& name) const { return check_value()->get_method(name); }
	// wcontext: none yet | transparent
	/*virtual*/ VClass *get_class() const { return fvalue?fvalue->get_class():0; }
	// wcontext: none yet | transparent
	/*virtual*/ bool is_or_derived_from(VClass& ancestor) { return fvalue?fvalue->is_or_derived_from(ancestor):false; }

public: // usage

	WContext(Pool& apool, Value *avalue) : Value(apool), 
		fvalue(avalue),
		string(apool) {
	}

	// appends a string to result
	// until Value written, ignores afterwards
	void write(String& astring);
	// if value.string!=0 writes string altogether[for showing class names, etc]
	// writes Value; raises an error if already
	void write(Value& avalue);
	//void write(String_iterator& from, String_iterator& to);

	// retrives the resulting value
	// that can be Text if value==0 or the Value object
	Value *value() const {
		return fvalue?fvalue:0;//TODO: new Text(string);
	}
	
private:
	String string;
	Value *fvalue;

private:
	// raises an exception on 0 value
	Value *check_value() const {
		if(!fvalue)
			pool().exception().raise(0,0,
				0,
				"accessing wcontext without value");

		return fvalue;
	}
};

#endif
