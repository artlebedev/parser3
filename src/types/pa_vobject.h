/**	@file
	Parser: @b object class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VOBJECT_H
#define PA_VOBJECT_H

static const char* IDENT_VOBJECT_H="$Date: 2002/08/21 10:18:36 $";

#include "pa_vjunction.h"
#include "pa_vclass.h"
#include "pa_vstateless_object.h"

// defines

#define CLASS_NAME "CLASS"
#define BASE_NAME "BASE"

/**	parser class instance, stores 
	- class VObject::fclass;
	- fields VObject::ffields (dynamic, not static, which are stored in class).
	- links to base/derived instances [VObject::fbase, VObject::fderived]
*/
class VObject: public VStateless_object {
public: // Value
	
	const char *type() const { return fclass.name_cstr(); }
	/*override*/ Value *as(const char *atype, bool looking_up);

	/// VObject: fclass
	VStateless_class *get_class() { return &fclass; }
	/*override*/ Value *base_object() { return fbase; }

	/*override*/ bool is_defined() const;
	/*override*/ Value *as_expr_result(bool);
	/*override*/ int as_int() const;
	/*override*/ double as_double();
	/*override*/ bool as_bool() const;
	/*override*/ VFile *as_vfile(String::Untaint_lang /*lang*/=String::UL_UNSPECIFIED, bool /*origins_mode*/=false);
	
	/*override*/ Hash *get_hash(const String *source);
	/*override*/ Table *get_table();

	/*override*/ Value *get_element(const String& aname, Value *aself, bool /*looking_up*/);
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);

	/// VObject: remember derived [the only client] */
	/*override*/ VObject *set_derived(VObject *aderived) { 
		VObject *result=fderived;
		fderived=aderived;
		return result;
	}

public: // creation

	VObject(Pool& apool, VStateless_class& aclass) : VStateless_object(apool), 
		fclass(aclass),
		ffields(apool),
		fderived(0),
		fbase(fclass.base_class()?fclass.base_class()->create_new_value(apool):0) {
		if(fbase)
			fbase->set_derived(this);
	}

private:

	Value *get_last_derived() {
		return fderived?fderived->get_last_derived():this;
	}

private:

	VStateless_class& fclass;
	Hash ffields;
	VObject *fderived;
	Value *fbase;
};

class Temp_derived {
	Value& fvalue;
	VObject *fsaved_derived;
public:
	Temp_derived(Value& avalue, VObject *aderived) : 
		fvalue(avalue),
		fsaved_derived(avalue.set_derived(aderived)) {}
	~Temp_derived() { fvalue.set_derived(fsaved_derived); }
};

#endif
