/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_vhash.h,v 1.2 2001/03/11 08:12:12 paf Exp $
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

	VHash(Pool& apool) : Value(apool), 
		fvalue(apool) {}

private:
	Hash fvalue;
};

#endif
