/*
  $Id: pa_wcontext.h,v 1.1 2001/02/21 16:11:49 paf Exp $
*/

/*
	data core
*/

#ifndef PA_WCONTEXT_H
#define PA_WCONTEXT_H

#include "pa_value.h"

class WContext : public Value {
public:
	WContext(Pool& apool) : Pooled(apool), string(apool) {}

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
		return value?value:new Text(string);
	}
	
public: //implement by replicating to value->calls

	const char *get_type() const { return "WContext"; }

	String *get_string() const { return &string };
	void put_string(const String *astring) { check(value)->put_string(astring); };

	Method_ref *get_method_ref() const { return check(value)->get_method_ref(); }

	Value *get_element(const String& name) const { return check(value)->get_element(name); }
	void put_element(const String& name, const Value *avalue){ check(value)->put_element(name, avalue); }

	Method *get_method(const String& name) const { return check(value)->get_method(name); }

	Class *get_class() const { return value?value->get_class():0; }
	bool has_parent(Class *aparent) { return value?value->has_parent(aparent):false; }

private:
	String string;
	Value *value;

private:
	// raises an exception on 0 value
	Value *check(const Value *value);
};

#endif
