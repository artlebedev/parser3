/*
  $Id: pa_context.h,v 1.1 2001/02/11 19:35:38 paf Exp $
*/

/*
	data core
*/

#ifndef PA_CONTEXT_H
#define PA_CONTEXT_H

#include "pa_value.h"

typedef Value RContext;

class WContext : public Value {
public:

	// appends a string to result
	// until Value written, ignores afterwards
	void write(String& astring);
	// if value.string!=0 writes just string, not Value
	// raises an error if already
	void write(Value& avalue);

	// retrives the resulting value
	// that can be Text if value==0 or the Value object
	Value *value() const {
		return value?value:new Text(string);
	}
	
public: //implement by replicating to value->calls

	virtual String *get_string() const { return &string };
	virtual void put_string(const String *astring) { check(value)->put_string(astring); };

	virtual Value *get_element(const String& name) const { return check(value)->get_element(name); }
	virtual void put_element(const String& name, const Value *avalue){ check(value)->put_element(name, avalue); }

	virtual Value *get_THIS() const { return check(value)->get_THIS(); }
	virtual String *get_code(const String& name) const { return check(value)->get_code(name); }
	virtual String *get_code() const { return check(value)->get_code(); }

private:
	String& string;
	Value *value;

private:
	// raises an exception on 0 value
	Value *check(const Value *value);
};

#endif
