/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMETHOD_FRAME_H
#define PA_VMETHOD_FRAME_H

static const char* IDENT_VMETHOD_FRAME_H="$Date: 2002/09/10 12:02:24 $";

#include "pa_wcontext.h"
#include "pa_vvoid.h"
#include "pa_vjunction.h"
#include "pa_request.h"

/**	Method frame write context
	accepts values written by method code
	also handles method parameters and local variables
*/
class VMethodFrame : public WContext {
public: // Value

	const char *type() const { return "method_frame"; }

	/// VMethodFrame: $result | parent get_string(=accumulated fstring)
	const String *get_string() { 
		// check the $result value
		Value *result=get_result_variable();
		// if we have one, return it's string value, else return as usual: accumulated fstring or fvalue
		return result ? result->get_string() : WContext::get_string();
	}
	
	/// VMethodFrame: my or self_transparent
	Value *get_element(const String& aname, Value *aself, bool looking_up) { 
		if(junction.method->max_numbered_params_count==0) {
			if(Value *result=static_cast<Value *>(my.get(aname)))
				return result;
		}
		return fself->get_element(aname, aself, looking_up); 
	}
	/// VMethodFrame: my or self_transparent
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace) { 
		if(junction.method->max_numbered_params_count==0 && my.put_replace(aname, avalue))
			return true;

		return fself->put_element(aname, avalue, replace);
	}

	/// VMethodFrame: self_transparent
	VStateless_class* get_class() { return fself->get_class(); }

public: // WContext

	/* override */ StringOrValue result() {
		// check the $result value
		Value *result_value=get_result_variable();
		// if we have one, return it, else return as usual: accumulated fstring or fvalue
		return result_value ? StringOrValue(0, result_value) : WContext::result();
	}

public: // usage

	VMethodFrame(Pool& apool, 
		const String& aname,
		const Junction& ajunction/*info: always method-junction*/) : 
		WContext(apool, 0 /* empty */, 0 /* no parent, junctions can be reattached only up to VMethodFrame */ ),

		fname(aname),
		junction(ajunction),
		store_param_index(0),

		my(apool),
		fnumbered_params(apool, aname),

		fself(0),
		fresult_initial_void(0) {

		if(has_my()) { // this method uses named params?
			const Method &method=*junction.method;
			if(method.locals_names) { // are there any local var names?
				// remember them
				// those are flags that fname is local == to be looked up in 'my'
				for(int i=0; i<method.locals_names->size(); i++) {
					// speedup: not checking for clash with "result" fname
					Value *value=NEW VVoid(pool());
					const String& fname=*method.locals_names->get_string(i);
					set_my_variable(fname, value);
				}
			}
			{ // always there is one local: $result
				fresult_initial_void=NEW VVoid(pool());
				set_my_variable(*result_var_name, fresult_initial_void);
			}
		}
	}

	const String& name() { return fname; }

	void set_self(Value& aself) { fself=&aself; }
	Value *self() { return fself; }

	bool can_store_param() {
		const Method& method=*junction.method;
		return method.params_names && store_param_index<method.params_names->size();
	}
	void store_param(Value *value) {
		const Method& method=*junction.method;
		int max_params=
			method.max_numbered_params_count?method.max_numbered_params_count:
			method.params_names?method.params_names->size():
			0;
		if(store_param_index==max_params)
			throw Exception("parser.runtime",
				&name(),
				"method of %s (%s) accepts maximum %d parameter(s)", 
					junction.self.get_class()->name_cstr(),
					junction.self.type(),
					max_params);
		
		if(method.max_numbered_params_count) { // are this method params numbered?
			fnumbered_params+=value;
		} else { // named param
			// speedup: not checking for clash with "result" fname
			const String& fname=*method.params_names->get_string(store_param_index);
			set_my_variable(fname, value);
		}
		store_param_index++;
	}
	void fill_unspecified_params() {
		const Method &method=*junction.method;
		if(method.params_names) // there are any named parameters might need filling?
			for(; store_param_index<method.params_names->size(); store_param_index++) {
				const String& fname=*method.params_names->get_string(store_param_index);
				my.put(fname, NEW VVoid(pool()));
			}
	}

	MethodParams *numbered_params() { return &fnumbered_params; }

private:

	bool has_my() {
		return junction.method->max_numbered_params_count==0;
	}

	void set_my_variable(const String& fname, Value *value) {
		my.put(fname, value); // remember param
	}

	Value *get_result_variable() {
		Value *result=has_my()?static_cast<Value*>(my.get(*result_var_name)):0;
		return result && result!=fresult_initial_void ? result : 0;
	}

public:
	
	const Junction& junction;

private:

	const String& fname;

	int store_param_index;
	Hash my;/*OR*/MethodParams fnumbered_params;
	Value *fself;

private:

	Value *fresult_initial_void;

};

#endif
