/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

static const char* IDENT_VHASH_H="$Date: 2003/07/24 11:31:26 $";

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_globals.h"

// defines

#define VHASH_TYPE "hash"
#define HASH_FIELDS_NAME "fields"

// externs

extern Methoded* hash_class;
extern const String hash_default_element_name;

// forwards

class VHash_lock;

/// value of type 'hash', implemented with Hash
class VHash: public VStateless_object {
	friend class VHash_lock;
public: // value

	override const char* type() const { return VHASH_TYPE; }
	override VStateless_class *get_class() { return hash_class; }

	/// VHash: finteger
	override int as_int() const { return fhash.count(); }
	/// VHash: finteger
	override double as_double() const { return as_int(); }
	/// VHash: count!=0
	override bool is_defined() const { return as_int()!=0; }
	/// VHash: count!=0
	override bool as_bool() const { return is_defined(); }
	/// VHash: count
	override Value& as_expr_result(bool ) { return *new VInt(as_int()); }

	/// VHash: fhash
	override HashStringValue *get_hash() { return &hash(); }

	/// VHash: (key)=value
	override Value* get_element(const String& aname, Value& aself, bool looking_up) { 
		// $element
		if(Value* result=fhash.get(aname))
			return result;

		// $fields -- pseudo field to make 'hash' more like 'table'
		if(aname == HASH_FIELDS_NAME)
			return this;

		// $method
		if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
			return result;

		// default value
		return get_default();
	}
	
	/// VHash: (key)=value
	override bool put_element(const String& aname, Value* avalue, bool /*replace*/) { 
		if(flocked) {
			if(!fhash.put_replace(aname, avalue))
				throw Exception("parser.runtime",
					&aname,
					"can not insert new hash key (hash flocked)");
		} else
			fhash.put(aname, avalue);

		return true;
	}

public: // usage

	VHash(): flocked(false) {}

	VHash(const HashStringValue& source): fhash(source), flocked(false) {}

	HashStringValue& hash() { 
		check_lock();
		return fhash; 
	}

	void set_default(Value* adefault) { 
		fhash.put(hash_default_element_name, adefault);
	}
	Value* get_default() { 
		return fhash.get(hash_default_element_name); 
	}

	void check_lock() {
		if(flocked)
			throw Exception("parser.runtime",
				0,
				"can not modify hash (flocked)");
	}

private:

	bool flocked;
	HashStringValue fhash;

};

class VHash_lock {
	VHash& fhash;
	bool saved;
public:
	VHash_lock(VHash& ahash): fhash(ahash) {
		saved=fhash.flocked;
		fhash.flocked=true;
	}
	~VHash_lock() {
		fhash.flocked=saved;
	}

};

#endif
