/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhash.h,v 1.14 2001/05/08 10:39:23 paf Exp $
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"

extern Methoded *hash_base_class;

/// value of type 'hash', implemented with Hash
class VHash : public VStateless_class {
public: // value

	const char *type() const { return "hash"; }

	/// VHash: fhash
	Hash *get_hash() { return &fhash; }

	/// VHash: 0
	VStateless_class *get_class() { return 0; }

	/// VHash: (key)=value
	Value *get_element(const String& name) { 
		// $CLASS,$BASE,$method
		if(Value *result=VStateless_class::get_element(name))
			return result;

		// $element
		Value *result=static_cast<Value *>(fhash.get(name));
		return result?result:fdefault;
	}
	
	/// VHash: (key)=value
	void put_element(const String& name, Value *value) { 
		fhash.put(name, value);
	}

public: // usage

	VHash(Pool& apool) : VStateless_class(apool, hash_base_class), 
		fhash(apool), 
		fdefault(0) {
	}

	Hash& hash() { return fhash; }

	void set_default(Value& adefault) { fdefault=&adefault; }
	Value *get_default() { return fdefault; }

private:

	Hash fhash;
	Value *fdefault;

};

#endif
