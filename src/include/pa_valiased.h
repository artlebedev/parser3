/*
  $Id: pa_valiased.h,v 1.1 2001/03/06 12:22:57 paf Exp $
*/

#ifndef PA_VALIASED_H
#define PA_VALIASED_H

#include "pa_value.h"

class Temp_alias;

class VAliased : public Value {
	friend Temp_alias;
public: // creation

	VAliased(Pool& apool, VClass& aclass_alias) : 
		Value(apool), 
		class_alias(&aclass_alias) {
	}

	// valiased: this
	VAliased *get_aliased() { return this; }

private: // alias handling

	// VAliased replacement mechanism is 'protected' from direct usage
	// Temp_alias object enforces paired set/restore
	VClass *set_alias(VClass *aclass_alias){
		VClass *result=class_alias;
		class_alias=aclass_alias;
		return result;
	}
	void restore_alias(VClass *aclass_alias) {
		class_alias=aclass_alias;
	}

protected:

	VClass *class_alias;
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
