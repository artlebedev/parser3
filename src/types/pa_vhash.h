/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhash.h,v 1.11 2001/04/26 14:55:35 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "pa_value.h"
#include "pa_hash.h"

/// value of type 'hash', implemented with Hash
class VHash : public Value {
public: // value

	/// all: for error reporting after fail(), etc
	const char *type() const { return "hash"; }

	/// VHash: fhash
	Hash *get_hash() { return &fhash; }

	/// VHash: (key)=value
	Value *get_element(const String& name) { 
		return static_cast<Value *>(fhash.get(name));
	}
	
	/// VHash: (key)=value
	void put_element(const String& name, Value *value) { 
		fhash.put(name, value);
	}

public: // usage

	VHash(Pool& apool) : Value(apool), 
		fhash(apool) {}

	Hash& hash() { return fhash; }

private:

	Hash fhash;

};

#endif
