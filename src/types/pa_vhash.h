/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhash.h,v 1.4 2001/03/13 17:17:29 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "pa_value.h"
#include "pa_hash.h"

class VHash : public Value {
public: // value

	// all: for error reporting after fail(), etc
	const char *type() const { return "hash"; }

	// value: (key)=value
	Value *get_element(const String& name) { 
		return static_cast<Value *>(fvalue.get(name));
	}
	
	// value: (key)=value
	void put_element(const String& name, Value *value) { 
		fvalue.put(name, value);
	}

	// value: size!=0
	bool get_bool() { return fvalue.size()!=0; }

public: // usage

	VHash(Pool& apool, Hash *ahash=0) : Value(apool), 
		fvalue(ahash?*ahash:*new(apool) Hash(apool)) {}

private:
	Hash& fvalue;
};

#endif
