/*
  $Id: pa_vhash.h,v 1.1 2001/02/22 16:21:49 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "pa_value.h"

class VHash : public Value {
public: // value

	// all: for error reporting after fail(), etc
	const char *type() const { return "Hash"; }

	// hash: (key)=value
	Value *get_element(const String& name) { 
		return static_cast<Value *>(hash.get(name));
	}
	
	// hash: (key)=value
	void put_element(const String& name, Value *value) { 
		hash.put(name, value);
	}

public: // usage

	VHash(Pool& apool) : Value(apool), hash(apool) {}

private:
	Hash hash;
};

#endif
