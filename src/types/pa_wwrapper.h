/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_wwrapper.h,v 1.5 2001/03/21 14:06:54 paf Exp $
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

#include "pa_wcontext.h"
#include "pa_exception.h"

class WWrapper : public WContext {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "wwrapper"; }
	// wwrapper: transparent
	Value *get_element(const String& name) { return check_value()->get_element(name); }
	// wwrapper: transparent
	void put_element(const String& aname, Value *avalue){ 
		if(!fvalue) {
			fvalue=NEW VHash(pool());
			// not constructing anymore [if were constructing]
			// so to allow method calls after real constructor-method call
			// sample:
			//	$hash[
			//		$key1[$i]
			//		^i.inc[]  ^rem{allow such calls}
			//		$key2[$1]
			fconstructing=false;

			String& auto_created_hash_name=*NEW String(pool());
			auto_created_hash_name.append(aname, String::UL_NO);
			fvalue->set_name(auto_created_hash_name.APPEND_CONST(" storage"));
		}

		fvalue->put_element(aname, avalue); 
	}

public: // usage

	WWrapper(Pool& apool, Value *avalue, bool aconstructed) : 
		WContext(apool, avalue, aconstructed) {
	}
	
private:
	// raises an exception on 0 value
	Value *check_value() const {
		if(!fvalue)
			THROW(0,0,
				0,
				"accessing wrapper without value");

		return fvalue;
	}
};

#endif
