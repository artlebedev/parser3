/**	@file
	Parser: @b write_wrapper write context

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

static const char* IDENT_WWRAPPER_H="$Date: 2002/08/07 13:24:31 $";

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
			//		$.key1[$i]
			//		^i.inc[]  ^rem{allow such calls}
			//		$.key2[$1]
			set_constructing(false);
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
			throw Exception(0,
				0,
				"accessing wrapper without value");

		return fvalue;
	}
};

#endif
