/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vmethod_frame.h,v 1.16 2001/09/26 10:32:26 parser Exp $
*/

#ifndef PA_VMETHOD_FRAME_H
#define PA_VMETHOD_FRAME_H

#include "pa_wcontext.h"
#include "pa_vvoid.h"
#include "pa_vjunction.h"

/**	Method frame write context
	accepts values written by method code
	also handles method parameters and local variables
*/
class VMethodFrame : public WContext {
public: // Value

	const char *type() const { return "method_frame"; }
	/// VMethodFrame: my or self_transparent
	Value *get_element(const String& name) { 
		if(my) {
			Value *result=static_cast<Value *>(my->get(name));
			if(result)
				return result;
		}
		return fself->get_element(name); 
	}
	/// VMethodFrame: my or self_transparent
	void put_element(const String& name, Value *value){ 
		if(!(my && my->put_replace(name, value)))
			fself->put_element(name, value);
	}

	/// VMethodFrame: self_transparent
	VStateless_class* get_class() { return fself->get_class(); }

	/// VMethodFrame: self_transparent
	VAliased *get_aliased() { return fself->get_aliased(); }

public: // wcontext

	Value *result() {
		// check the $result value
		Value *result=my?static_cast<Value*>(my->get(*result_var_name)):0;
		// if we have one, return it, else return as usual: accumulated fstring or fvalue
		return result && (result!=fresult_initial_void) ?result:WContext::result();
	}

public: // usage

	VMethodFrame(Pool& apool, 
		const String& name,
		const Junction& ajunction/*info: always method-junction*/) : 
		WContext(apool, 0 /* empty */),

		junction(ajunction),
		store_param_index(0),
		fself(0),
		fresult_initial_void(0) {
		set_name(name);

		const Method &method=*junction.method;

		if(method.max_numbered_params_count) { // are this method params numbered?
			my=0; // no named parameters
			fnumbered_params=NEW MethodParams(pool(), name); // create storage
		} else { // named params
			my=NEW Hash(pool()); // create storage
			fnumbered_params=0; // no numbered parameters
			
			if(method.locals_names) { // are there any local var names?
				// remember them
				// those are flags that name is local == to be looked up in 'my'
				for(int i=0; i<method.locals_names->size(); i++) {
					// speedup: not checking for clash with "result" name
					Value *value=NEW VVoid(pool());
					const String& name=*method.locals_names->get_string(i);
					my->put(name, value);
					value->set_name(name);
				}
			}
			{ // always there is one local: $result
				fresult_initial_void=NEW VVoid(pool());
				my->put(*result_var_name, fresult_initial_void);
				fresult_initial_void->set_name(*result_var_name);
			}
		}
	}

	void set_self(Value& aself) { fself=&aself; }
	Value *self() { return fself; }

	void store_param(const String& actual_method_name, Value *value) {
		const Method& method=*junction.method;
		int max_params=
			method.max_numbered_params_count?method.max_numbered_params_count:
			method.params_names?method.params_names->size():
			0;
		if(store_param_index==max_params)
			THROW(0,0,
				&actual_method_name,
				"method of %s (%s) accepts maximum %d parameter(s)", 
					junction.self.name().cstr(),
					junction.self.type(),
					max_params);
		
		if(method.max_numbered_params_count) { // are this method params numbered?
			*fnumbered_params+=value;
		} else { // named param
			// speedup: not checking for clash with "result" name
			const String& name=*method.params_names->get_string(store_param_index);
			my->put(name, value); // remember param
			value->set_name(name); // set param's 'name'
		}
		store_param_index++;
	}
	void fill_unspecified_params() {
		const Method &method=*junction.method;
		if(method.params_names) // there are any named parameters might need filling?
			for(; store_param_index<method.params_names->size(); store_param_index++) {
				Value *value=NEW VVoid(pool());
				const String& name=*method.params_names->get_string(store_param_index);
				my->put(name, value);
				value->set_name(name);
			}
	}

	MethodParams *numbered_params() { return fnumbered_params; }

public:
	
	const Junction& junction;

private:
	int store_param_index;
	Hash *my;/*OR*/MethodParams *fnumbered_params;
	Value *fself;

private:
	Value *fresult_initial_void;

};

#endif
