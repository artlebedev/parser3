/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vhash.h,v 1.17 2001/09/18 13:31:56 parser Exp $
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

	/// VHash: count!=0
	bool is_defined() const { return hash().size()!=0; }

	/// VHash: fhash
	Hash *get_hash() { return &fhash; }

	/// VHash: 0
	VStateless_class *get_class() { return 0; }

	/// VHash: (key)=value
	Value *get_element(const String& name) { 
		// $CLASS,$method
		if(Value *result=VStateless_class::get_element(name))
			return result;

		// $element
		if(Value *result=static_cast<Value *>(fhash.get(name)))
			return result;

		// default value
		return static_cast<Value *>(fhash.get(*hash_default_element_name));
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
