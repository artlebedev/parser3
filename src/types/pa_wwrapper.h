/**	@file
	Parser: @b write_wrapper write context

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_wwrapper.h,v 1.11 2001/09/26 10:32:26 parser Exp $
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

#include "pa_wcontext.h"
#include "pa_exception.h"

/// specialized write context, adds to WContext VHash autocreation ability
class WWrapper : public WContext {
public: // Value

	const char *type() const { return "wwrapper"; }
	/// WWrapper: transparent
	Value *get_element(const String& name) { return check_value()->get_element(name); }
	/// WWrapper: transparent
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

			String& auto_created_hash_name=*NEW String(aname);
			fvalue->set_name(auto_created_hash_name.APPEND_CONST(" storage"));
		}

		fvalue->put_element(aname, avalue); 
	}

public: // usage

	WWrapper(Pool& apool, Value *avalue) : 
		WContext(apool, avalue) {
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
