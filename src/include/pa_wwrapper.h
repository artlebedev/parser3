/*
  $Id: pa_wwrapper.h,v 1.10 2001/03/07 13:54:47 paf Exp $
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
	Value *get_element(String& name) { return check_value()->get_element(name); }
	// wwrapper: transparent
	void put_element(const String& name, Value *avalue){ 
		if(!fvalue)
			fvalue=NEW VHash(pool());
		fvalue->put_element(name, avalue); 
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
