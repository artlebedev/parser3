/*
  $Id: pa_vframe.h,v 1.1 2001/02/23 17:12:57 paf Exp $
*/

#ifndef PA_VFRAME_H
#define PA_VFRAME_H

#include "pa_wcontext.h"
#include "pa_vunknown.h"

class VFrame : public WContext {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "VFrame"; }
	// frame: my or self_transparent
	Value *get_element(const String& name) { 
		Value *result=static_cast<Value *>(my.get(name));
		if(!result)
			result=self->get_element(name);
		return result; 
	}
	// frame: my or self_transparent
	void put_element(const String& name, Value *value){ 
		if(!my.replace(name, value))
			self->put_element(name, value);
	}

	// frame: transparent
	Method *get_method(const String& name) const { return self->get_method(name); }
	// frame: none yet | transparent
	VClass *get_class() const { return self->get_class(); }
	// frame: none yet | transparent
	bool is_or_derived_from(VClass& ancestor) { return self->is_or_derived_from(ancestor); }

public: // usage

	VFrame(Pool& apool, Method& amethod) : WContext(apool, 0 /* empty */),
		method(amethod),
		store_param_index(0),
		my(apool),
		self(0) {
		// those are flags that name is local == to be looked up in 'my'
		for(int i=0; i<method.locals_names.size(); i++) {
			my.put(
				*static_cast<String *>(method.locals_names.get(i)), 
				NEW VUnknown(pool()));
		}
	}

	void set_self(Value *aself) { self=aself; }

	void store_param(Value *value) {
		if(params_filled())
			THROW(0,0,
				name(),
				"call: too many params (max=%d)", method.params_names.size());
		
		my.put(*static_cast<String *>(method.params_names.get(store_param_index++)), value);
	}
	bool params_filled() {
		return store_param_index==method.params_names.size();
	}

public:
	
	const Method& method;

private:
	int store_param_index;
	Hash my;
	Value *self;

};

#endif
