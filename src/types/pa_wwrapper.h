/**	@file
	Parser: @b write_wrapper write context

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

static const char* IDENT_WWRAPPER_H="$Date: 2003/07/24 11:31:27 $";

#include "pa_wcontext.h"
#include "pa_exception.h"

/// specialized write context, adds to WContext VHash autocreation ability
class WWrapper: public WContext {
public: // Value

	override const char* type() const { return "wwrapper"; }
	/// WWrapper: transparent
	override Value* get_element(const String& aname, Value& aself, bool looking_up) { 
		return as_value().get_element(aname, aself, looking_up); 
	}
	/// WWrapper: transparent
	override bool put_element(const String& aname, Value* avalue, bool replace) { 
		if(!fvalue) {
			fvalue=new VHash;
			// not constructing anymore [if were constructing]
			// so to allow method calls after real constructor-method call
			// sample:
			//	$hash[
			//		$.key1[$i]
			//		^i.inc[]  ^rem{allow such calls}
			//		$.key2[$1]
			set_constructing(false);
		}

		return fvalue->put_element(aname, avalue, replace); 
	}

public: // usage

	WWrapper(Value* avalue, WContext *aparent) : 
		WContext(avalue, aparent) {
	}
	
private:
	// raises an exception on 0 value
	Value& as_value() const {
		if(!fvalue)
			throw Exception(0,
				0,
				"accessing wrapper without value");

		return *fvalue;
	}
};

#endif
