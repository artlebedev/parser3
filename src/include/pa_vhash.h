/*
  $Id: pa_vhash.h,v 1.8 2001/03/08 17:08:13 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "pa_value.h"
#include "pa_hash.h"

class VHash : public Value {
public: // value

	// all: for error reporting after fail(), etc
	const char *type() const { return "fvalue"; }
	// clone
	Value *clone() const { return NEW VHash(pool(), fvalue); };

	// fvalue: (key)=value
	Value *get_element(const String& name) { 
		return static_cast<Value *>(fvalue.get(name));
	}
	
	// fvalue: (key)=value
	void put_element(const String& name, Value *value) { 
		fvalue.put(name, value);
	}

	// fvalue: size!=0
	bool get_bool() { return fvalue.size()!=0; }

public: // usage

	VHash(Pool& apool) : Value(apool), 
		fvalue(*new(apool) Hash(apool)) {}

	VHash(Pool& apool, Hash& avalue) : Value(apool), 
		fvalue(avalue) {}

private:
	Hash& fvalue;
};

#endif
