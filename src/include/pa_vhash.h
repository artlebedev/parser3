/*
  $Id: pa_vhash.h,v 1.3 2001/03/06 15:02:46 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "pa_value.h"
#include "pa_hash.h"

class VHash : public Value {
public: // value

	// all: for error reporting after fail(), etc
	const char *type() const { return "hash"; }

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
