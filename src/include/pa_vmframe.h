/*
  $Id: pa_vmframe.h,v 1.17 2001/03/08 15:32:51 paf Exp $
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
		Value *result=static_cast<Value *>(my->get(name));
		if(!result)
			result=fself->get_element(name);
		return result; 
	}
	// frame: my or self_transparent
	void put_element(const String& name, Value *value){ 
		if(!my->put_replace(name, value))
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
		my(0), fself(0) {

		Method &method=*junction.method;

		if(method.numbered_params_count) // are this method params numbered?
			fnumbered_params=NEW Array(pool()); // create storage
		else // named params
			my=NEW Hash(pool()); // create storage

		if(method.locals_names) { // there are any local var names?
			// remember them
			// those are flags that name is local == to be looked up in 'my'
			for(int i=0; i<method.locals_names->size(); i++) {
				Value *value=NEW VUnknown(pool());
				String& name=*static_cast<String *>(method.locals_names->get(i));
				value->set_name(name);
				my->put(name, value);
			}
		}
	}

	void set_self(Value& aself) { fself=&aself; }

	void store_param(Value *value) {
		Method& method=*junction.method;
		int max_params=
			method.numbered_params_count?method.numbered_params_count:
			method.params_names?method.params_names->size():
			0;
		if(store_param_index==max_params)
			THROW(0,0,
				&junction.self.name(),
				"%s method '%s' accepts maximum %d parameters", 
					junction.self.type(),
					method.name.cstr(),
					max_params);
		
		if(method.numbered_params_count) { // are this method params numbered?
			*fnumbered_params+=value;
		} else { // named params
			String& name=*static_cast<String *>(
				method.params_names->get(store_param_index++));
			my->put(name, value);
			value->set_name(name);
		}
	}
	void fill_unspecified_params() {
		Method &method=*junction.method;
		if(method.numbered_params_count) { // are this method params numbered?
			for(; store_param_index<method.numbered_params_count; store_param_index++) {
				Value *value=NEW VUnknown(pool());
				//value->set_name(/*"Param#" . store_param_index*/);
				*fnumbered_params+=value;
			}
		} else { // named params
			if(method.params_names) // there are any parameters might need filling?
				for(; store_param_index<method.params_names->size(); store_param_index++) {
					Value *value=NEW VUnknown(pool());
					String& name=*static_cast<String *>(method.params_names->get(store_param_index));
					value->set_name(name);
					my->put(name, value);
				}
		}
	}

	Array& numbered_params() { return *fnumbered_params; }

public:
	
	const Junction& junction;

private:
	int store_param_index;
	Hash *my;/*OR*/Array *fnumbered_params;
	Value *fself;

};

#endif
