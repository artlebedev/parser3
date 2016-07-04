/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#define IDENT_PA_VHASH_H "$Id: pa_vhash.h,v 1.75 2016/07/04 17:26:23 moko Exp $"

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_globals.h"
#include "pa_symbols.h"

// defines

#define VHASH_TYPE "hash"

extern Methoded* hash_class;

/// value of type 'hash', implemented with Hash
class VHash: public VStateless_object {
public: // value

	override const char* type() const { return VHASH_TYPE; }
	override VStateless_class *get_class() { return hash_class; }

	/// VHash: finteger
	override int as_int() const { return fhash.count(); }
	/// VHash: finteger
	override double as_double() const { return fhash.count(); }
	/// VHash: count!=0
	override bool is_defined() const { return fhash.count()!=0; }
	/// VHash: count!=0
	override bool as_bool() const { return fhash.count()!=0; }
	/// VHash: count
	override Value& as_expr_result() { return *new VInt(as_int()); }

	/// VHash: fhash
	override HashStringValue *get_hash() { return &hash(); }

	override HashStringValue* get_fields() { return &fhash; }

	/// VHash: (key)=value
	override Value* get_element(const String& aname) { 
		// $element first
		if(Value* result=fhash.get(aname))
			return result;

		// $fields -- pseudo field to make 'hash' more like 'table'
		if(SYMBOLS_EQ(aname,FIELDS_SYMBOL))
			return this;

#if !defined(FEATURE_GET_ELEMENT4CALL) || !defined(OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		// $method, CLASS, CLASS_NAME
		if(Value* result=VStateless_object::get_element(aname))
			return result;
#endif

		// default value
		return get_default();
	}
	
#ifdef FEATURE_GET_ELEMENT4CALL
	override Value* get_element4call(const String& aname) {
		// $method first
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		// $element
		if(Value* result=fhash.get(aname))
			return result;

		// default value
		return get_default();
	}
#endif
	
	/// VHash: (key)=value
	override const VJunction* put_element(const String& aname, Value* avalue) {
		if(SYMBOLS_EQ(aname,_DEFAULT_SYMBOL))
			set_default(avalue);
		else 
			fhash.put(aname, avalue);

		return PUT_ELEMENT_REPLACED_ELEMENT;
	}

	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);

public: // usage

	VHash(): _default(0) {}

	VHash(const HashStringValue& source): fhash(source), _default(0) {}

	HashStringValue& hash() { 
		return fhash; 
	}

	void set_default(Value* adefault) { 
		_default=adefault;
	}

	Value* get_default() { 
		return _default;
	}

	void extract_default();

private:

	HashStringValue fhash;
	Value* _default;

};

#endif
