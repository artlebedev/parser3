/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

static const char* IDENT_VHASH_H="$Date: 2002/08/13 15:55:43 $";

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"

// externs

extern Methoded *hash_class;

// forwards

class VHash_lock;

/// value of type 'hash', implemented with Hash
class VHash : public VStateless_object {
	friend class VHash_lock;
public: // value

	const char *type() const { return "hash"; }
	VStateless_class *get_class() { return hash_class; }

	/// VHash: finteger
	int as_int() const { return fhash.size(); }
	/// VHash: finteger
	double as_double() const { return as_int(); }
	/// VHash: count!=0
	bool is_defined() const { return as_int()!=0; }
	/// VHash: count!=0
	bool as_bool() const { return is_defined(); }
	/// VHash: count
	Value *as_expr_result(bool ) { return NEW VInt(pool(), as_int()); }

	/// VHash: fhash
	Hash *get_hash(const String *source) { return &hash(source); }

	/// VHash: (key)=value
	Value *get_element(const String& aname, Value *aself, bool looking_down) { 
		// $method
		if(Value *result=VStateless_object::get_element(aname, aself, looking_down))
			return result;

		// $element
		if(Value *result=static_cast<Value *>(fhash.get(aname)))
			return result;

		// default value
		return get_default();
	}
	
	/// VHash: (key)=value
	/*override*/ bool put_element(const String& aname, Value *avalue, bool /*replace*/) { 
		hash(&aname).put(aname, avalue);

		return true;
	}

public: // usage

	VHash(Pool& apool) : VStateless_object(apool), 
		fhash(apool), locked(false) {
	}

	VHash(Pool& apool, const Hash& source) : VStateless_object(apool), 
		fhash(source), locked(false) {
	}

	Hash& hash(const String *source) { 
		check_lock(source);
		return fhash; 
	}

	void set_default(Value& adefault) { 
		hash(0).put(*hash_default_element_name, &adefault);
	}
	Value *get_default() { 
		return static_cast<Value *>(fhash.get(*hash_default_element_name)); 
	}

	void check_lock(const String *source) {
		if(locked)
			throw Exception("parser.runtime",
				source,
				"can not modify hash (locked)");
	}

private:

	bool locked;
	Hash fhash;

};

class VHash_lock {
	VHash& fhash;
	bool saved;
public:
	VHash_lock(VHash& ahash) : fhash(ahash) {
		saved=fhash.locked;
		fhash.locked=true;
	}
	~VHash_lock() {
		fhash.locked=saved;
	}

};

#endif
