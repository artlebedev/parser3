/** @file
	Parser: @b hash parser type decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASH_H
#define PA_VHASH_H

#define IDENT_PA_VHASH_H "$Id: pa_vhash.h,v 1.82 2020/12/15 17:10:40 moko Exp $"

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"
#include "pa_globals.h"
#include "pa_symbols.h"

// defines

#define VHASH_TYPE "hash"

extern Methoded* hash_class;

class VHashBase: public VStateless_object {
public: // value

	override const char* type() const { return VHASH_TYPE; }
	override VStateless_class *get_class() { return hash_class; }

public: // usage

	virtual HashStringValue& hash()=0;
	virtual void set_default(Value* adefault)=0;
	virtual Value* get_default()=0;
};


/// value of type 'hash', implemented with Hash
class VHash: public VHashBase {
public: // value

	/// VHash: count
	override int as_int() const { return fhash.count(); }
	override double as_double() const { return fhash.count(); }
	override bool is_defined() const { return fhash.count()!=0 || _default!=NULL; }
	override bool as_bool() const { return fhash.count()!=0; }
	override Value& as_expr_result() { return *new VInt(fhash.count()); }

	/// VHash: fhash
	override HashStringValue *get_hash() { return &fhash; }
	override HashStringValue* get_fields() { return &fhash; }
	override HashStringValue* get_fields_reference() { return &fhash; }

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

		return 0;
	}

	override VFile* as_vfile(String::Language lang, const Request_charsets *charsets=0);

public: // usage

	VHash(): _default(0) {}

	VHash(const HashStringValue& source): fhash(source), _default(0) {}

	override HashStringValue& hash() { return fhash; }
	override void set_default(Value* adefault) { _default=adefault; }
	override Value* get_default() { return _default; }

	void extract_default();

private:

	HashStringValue fhash;
	Value* _default;

};

/// value of type 'hash', implemented with Hash reference
class VHashReference: public VHashBase {
public: // value

	/// VHash: count
	override int as_int() const { return fhash->count(); }
	override double as_double() const { return fhash->count(); }
	override bool is_defined() const { return fhash->count()!=0; }
	override bool as_bool() const { return fhash->count()!=0; }
	override Value& as_expr_result() { return *new VInt(fhash->count()); }

	/// VHash: fhash
	override HashStringValue *get_hash() { return fhash; }
	override HashStringValue* get_fields() { return fhash; }
	override HashStringValue* get_fields_reference() { return fhash; }

	/// VHash: (key)=value
	override Value* get_element(const String& aname) { 
		// $element first
		if(Value* result=fhash->get(aname))
			return result;

		// $fields -- pseudo field to make 'hash' more like 'table'
		if(SYMBOLS_EQ(aname,FIELDS_SYMBOL))
			return this;

#if !defined(FEATURE_GET_ELEMENT4CALL) || !defined(OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL)
		// $method, CLASS, CLASS_NAME
		if(Value* result=VStateless_object::get_element(aname))
			return result;
#endif

		return 0;
	}
	
#ifdef FEATURE_GET_ELEMENT4CALL
	override Value* get_element4call(const String& aname) {
		// $method first
		if(Value* result=VStateless_object::get_element(aname))
			return result;

		// $element
		if(Value* result=fhash->get(aname))
			return result;

		return 0;
	}
#endif
	
	/// VHash: (key)=value
	override const VJunction* put_element(const String& aname, Value* avalue) {
		fhash->put(aname, avalue);
		return 0;
	}

public: // usage

	VHashReference(HashStringValue& source): fhash(&source) {}

	override HashStringValue& hash() { return *fhash; }
	override void set_default(Value*) { }
	override Value* get_default() { return 0; }

private:

	HashStringValue *fhash;
};

#endif
