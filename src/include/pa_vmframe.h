/*
  $Id: pa_vmframe.h,v 1.12 2001/03/07 13:54:47 paf Exp $
*/

#ifndef PA_VMFRAME_H
#define PA_VMFRAME_H

#include "pa_wcontext.h"
#include "pa_vunknown.h"
#include "pa_vjunction.h"

class VMethodFrame : public WContext {
public: // Value

	// all: for error reporting after fail(), etc
	const char *type() const { return "method_frame"; }
	// frame: my or self_transparent
	Value *get_element(String& name) { 
		Value *result=static_cast<Value *>(my.get(name));
		if(!result)
			result=self->get_element(name);
		return result; 
	}
	// frame: my or self_transparent
	void put_element(const String& name, Value *value){ 
		if(!my.put_replace(name, value))
			self->put_element(name, value);
	}

	// frame: self_transparent
	VClass* get_class() { return self->get_class(); }

	// methodframe: self_transparent
	VAliased *get_aliased() { return self->get_aliased(); }

public: // usage

	VMethodFrame(Pool& apool, const Junction& ajunction) : 
		WContext(apool, 0 /* empty */, false /* not constructing */),

		junction(ajunction),
		store_param_index(0),
		my(apool),
		self(0) {
		if(Method* method=junction.method) { // method junction?
			// remember local var names
			// those are flags that name is local == to be looked up in 'my'
			for(int i=0; i<method->locals_names.size(); i++) {
				my.put(
					*static_cast<String *>(method->locals_names.get(i)), 
					NEW VUnknown(pool()));
			}
		}
	}

	void set_self(Value& aself) { self=&aself; }

	void store_param(Value *value) {
		Method *method=junction.method;
		if(store_param_index==method->params_names.size())
			THROW(0,0,
				&junction.self.name(),
				"%s method '%s' accepts maximum %d parameters", 
					junction.self.type(),
					method->name.cstr(),
					method->params_names.size());
		
		String& name=*static_cast<String *>(method->params_names.get(store_param_index++));
		my.put(name, value);
		value->set_name(name);
	}
	void fill_unspecified_params() {
		Method *method=junction.method;
		for(; store_param_index<method->params_names.size(); store_param_index++)
			my.put(
				*static_cast<String *>(method->params_names.get(store_param_index)), 
				NEW VUnknown(pool()));
	}

public:
	
	const Junction& junction;

private:
	int store_param_index;
	Hash my;
	Value *self;

};

#endif
