/*
  $Id: pa_vmframe.h,v 1.22 2001/03/09 08:28:33 paf Exp $
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
	Value *get_element(const String& name) { 
		if(my) {
			Value *result=static_cast<Value *>(my->get(name));
			if(result)
				return result;
		}
		return fself->get_element(name); 
	}
	// frame: my or self_transparent
	void put_element(const String& name, Value *value){ 
		if(!(my && my->put_replace(name, value)))
			fself->put_element(name, value);
	}

	// frame: self_transparent
	VClass* get_class() { return fself->get_class(); }

	// methodframe: self_transparent
	VAliased *get_aliased() { return fself->get_aliased(); }

public: // usage

	VMethodFrame(Pool& apool, const Junction& ajunction/*info: always method-junction*/) : 
		WContext(apool, 0 /* empty */, false /* not constructing */),

		junction(ajunction),
		store_param_index(0),
		my(0), fnumbered_params(0),
		fself(0) {

		Method &method=*junction.method;

		if(method.max_numbered_params_count) // are this method params numbered?
			fnumbered_params=NEW Array(pool()); // create storage
		else // named params
			my=NEW Hash(pool()); // create storage

		if(method.locals_names) { // there are any local var names?
			// remember them
			// those are flags that name is local == to be looked up in 'my'
			for(int i=0; i<method.locals_names->size(); i++) {
				Value *value=NEW VUnknown(pool());
				String& name=*static_cast<String *>(method.locals_names->get(i));
				my->put(name, value);
				value->set_name(name);
			}
		}
	}

	void set_self(Value& aself) { fself=&aself; }

	void store_param(Value *value) {
		Method& method=*junction.method;
		int max_params=
			method.max_numbered_params_count?method.max_numbered_params_count:
			method.params_names?method.params_names->size():
			0;
		if(store_param_index==max_params)
			THROW(0,0,
				&junction.self.name(),
				"(%s) method '%s' accepts maximum %d parameter(s)", 
					junction.self.type(),
					method.name.cstr(),
					max_params);
		
		if(method.max_numbered_params_count) { // are this method params numbered?
			*fnumbered_params+=value;
		} else { // named param
			String& name=*static_cast<String *>(
				method.params_names->get(store_param_index++));
			my->put(name, value); // remember param
			value->set_name(name); // set param's 'name'
		}
	}
	void fill_unspecified_params() {
		Method &method=*junction.method;
		if(method.params_names) // there are any named parameters might need filling?
			for(; store_param_index<method.params_names->size(); store_param_index++) {
				Value *value=NEW VUnknown(pool());
				String& name=*static_cast<String *>(
					method.params_names->get(store_param_index));
				my->put(name, value);
				value->set_name(name);
			}
	}

	Array *numbered_params() { return fnumbered_params; }

public:
	
	const Junction& junction;

private:
	int store_param_index;
	Hash *my;/*OR*/Array *fnumbered_params;
	Value *fself;

};

#endif
