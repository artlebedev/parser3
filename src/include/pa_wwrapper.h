/*
  $Id: pa_wwrapper.h,v 1.7 2001/02/25 14:23:31 paf Exp $
*/

#ifndef PA_WWRAPPER_H
#define PA_WWRAPPER_H

#include "pa_wcontext.h"
#include "pa_exception.h"

class WWrapper : public WContext {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "WWrapper"; }
	// wwrapper: transparent
	Value *get_element(const String& name) { return check_value()->get_element(name); }
	// wwrapper: transparent
	void put_element(const String& name, Value *avalue){ 
		if(!fvalue)
			fvalue=NEW VHash(pool());
		fvalue->put_element(name, avalue); 
	}

	// wwrapper: none yet | transparent
	VClass *get_class() { return fvalue?fvalue->get_class():0; }

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
