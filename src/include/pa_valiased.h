/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: pa_valiased.h,v 1.3 2001/03/10 16:34:35 paf Exp $
*/

#ifndef PA_VALIASED_H
#define PA_VALIASED_H

#include "pa_value.h"

class Temp_alias;

class VAliased : public Value {
	friend Temp_alias;
public: // creation

	VAliased(Pool& apool, VClass& aclass_alias) : Value(apool), 
		fclass_alias(&aclass_alias) {
	}

	// valiased: this
	VAliased *get_aliased() { return this; }

private: // alias handling

	// VAliased replacement mechanism is 'protected' from direct usage
	// Temp_alias object enforces paired set/restore
	VClass *set_alias(VClass *aclass_alias){
		VClass *result=fclass_alias;
		fclass_alias=aclass_alias;
		return result;
	}
	void restore_alias(VClass *aclass_alias) {
		fclass_alias=aclass_alias;
	}

protected:

	VClass *fclass_alias;
};

class Temp_alias {
	VAliased& vobject;
	VClass *saved_alias;
public:
	Temp_alias(VAliased& avobject, VClass& alias) : 
		vobject(avobject),
		saved_alias(avobject.set_alias(&alias)) {
	}
	~Temp_alias() { 
		vobject.restore_alias(saved_alias); 
	}
};

#endif
