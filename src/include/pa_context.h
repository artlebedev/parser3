/*
  $Id: pa_context.h,v 1.4 2001/02/13 15:10:45 paf Exp $
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

	virtual Method_ref *get_method_ref() const { return check(value)->get_method_ref(); }

	virtual Value *get_element(const String& name) const { return check(value)->get_element(name); }
	virtual void put_element(const String& name, const Value *avalue){ check(value)->put_element(name, avalue); }

	virtual Method *get_method(const String& name) const { return check(value)->get_method(name); }

	virtual Class *get_class() const { return check(value)->get_class(); }

private:
	String& string;
	Value *value;

private:
	// raises an exception on 0 value
	Value *check(const Value *value);
};

#endif
