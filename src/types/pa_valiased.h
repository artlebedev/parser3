/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_valiased.h,v 1.5 2001/03/19 17:42:17 paf Exp $
*/

#ifndef PA_VALIASED_H
#define PA_VALIASED_H

#include "pa_value.h"

class Temp_alias;

class VAliased : public Value {
	friend Temp_alias;
public: // creation

	VAliased(Pool& apool, VStateless_class& aclass_alias) : Value(apool), 
		fclass_alias(&aclass_alias) {
	}

	// valiased: this
	VAliased *get_aliased() { return this; }

private: // alias handling

	// VAliased replacement mechanism is 'protected' from direct usage
	// Temp_alias object enforces paired set/restore
	VStateless_class *set_alias(VStateless_class *aclass_alias) {
		VStateless_class *result=fclass_alias;
		fclass_alias=aclass_alias;
		return result;
	}
	void restore_alias(VStateless_class *aclass_alias) {
		fclass_alias=aclass_alias;
	}

protected:

	VStateless_class *fclass_alias;
};

class Temp_alias {
	VAliased& vobject;
	VStateless_class *saved_alias;
public:
	Temp_alias(VAliased& avobject, VStateless_class& alias) : 
		vobject(avobject),
		saved_alias(avobject.set_alias(&alias)) {
	}
	~Temp_alias() { 
		vobject.restore_alias(saved_alias); 
	}
};

#endif
