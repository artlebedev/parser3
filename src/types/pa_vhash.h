/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhash.h,v 1.6 2001/03/18 11:37:54 paf Exp $
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
		return static_cast<Value *>(fhash.get(name));
	}
	
	// hash: (key)=value
	void put_element(const String& name, Value *value) { 
		fhash.put(name, value);
	}

public: // usage

	VHash(Pool& apool, Hash *ahash=0) : Value(apool), 
		fhash(ahash?*ahash:*new(apool) Hash(apool)) {}

private:

	Hash& fhash;

};

#endif
