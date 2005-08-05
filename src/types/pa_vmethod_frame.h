/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMETHOD_FRAME_H
#define PA_VMETHOD_FRAME_H

static const char * const IDENT_VMETHOD_FRAME_H="$Date: 2005/08/05 13:03:05 $";

#include "pa_wcontext.h"
#include "pa_vvoid.h"
#include "pa_vjunction.h"

// defines

#define CALLER_ELEMENT_NAME "caller"
#define SELF_ELEMENT_NAME "self"
#define RESULT_VAR_NAME "result"

// forwards

class Request;

/**
	@b method parameters passed in this array.
	contains handy typecast ad junction/not junction ensurers

*/
class MethodParams: public Array<Value*> {
public:
	Value& operator[] (size_t index) { return *get(index); }

	Value& last() { return *get(count()-1); }

	/// handy is-value-a-junction ensurer
	Value& as_junction(int index, const char* msg) { 
		return get_as(index, true, msg); 
	}
	/// handy value-is-not-a-junction ensurer
	Value& as_no_junction(int index, const char* msg) { 
		return get_as(index, false, msg); 
	}
	/// handy expression auto-processing to double
	double as_double(int index, const char* msg, Request& r) { 
		return get_processed(index, msg, r).as_double(); 
	}
	/// handy expression auto-processing to int
	int as_int(int index, const char* msg, Request& r) { 
		return get_processed(index, msg, r).as_int(); 
	}
	/// handy expression auto-processing to bool
	bool as_bool(int index, const char* msg, Request& r) { 
		return get_processed(index, msg, r).as_bool(); 
	}
	/// handy string ensurer
	const String& as_string(int index, const char* msg) { 
		return as_no_junction(index, msg).as_string();
	}

private:

	/// handy value-is/not-a-junction ensurer
	Value& get_as(int index, bool as_junction, const char* msg) { 
		Value* result=get(index);
		if((result->get_junction()!=0) ^ as_junction)
			throw Exception("parser.runtime",
				0,
				"%s (parameter #%d)", msg, 1+index);

		return *result;
	}

	Value& get_processed(int index, const char* msg, Request& r);

};

/**	Method frame write context
	accepts values written by method code
	also handles method parameters and local variables
*/
class VMethodFrame: public WContext {
	VMethodFrame *fcaller;

	size_t store_param_index;
	HashStringValue* my;/*OR*/MethodParams* fnumbered_params;
	Value* fself;

	Value* fresult_initial_void;

public: // Value

	override const char* type() const { return "method_frame"; }

	/// VMethodFrame: $result | parent get_string(=accumulated fstring)
	override const String* get_string() { 
		// check the $result value
		Value* result=get_result_variable();
		// if we have one, return it's string value, else return as usual: accumulated fstring or fvalue
		return result ? result->get_string() : WContext::get_string();
	}
	
	/// VMethodFrame: my or self_transparent or $caller
	override Value* get_element(const String& aname, Value& /*aself*/, bool looking_up) { 
		if(my) {
			if(Value* result=my->get(aname))
				return result;
		}
		if(Value* result=self().get_element(aname, self(), looking_up))
			return result;

		if(aname==CALLER_ELEMENT_NAME)
			return caller();

		if(aname==SELF_ELEMENT_NAME)
			return &self();

		return 0;
	}
	/// VMethodFrame: my or self_transparent
	override bool put_element(const String& aname, Value* avalue, bool replace) { 
		if(my && my->put_replace(aname, avalue))
			return true;

		return self().put_element(aname, avalue, replace);
	}

	/// VMethodFrame: self_transparent
	override VStateless_class* get_class() { return self().get_class(); }

	/// VMethodFrame: self_transparent
	override Value* base() { return self().base(); }

public: // WContext

	override StringOrValue result() {
		// check the $result value
		Value* result_value=get_result_variable();
		// if we have one, return it, else return as usual: accumulated fstring or fvalue
		return result_value ? StringOrValue(0, result_value) : WContext::result();
	}

	void write(Value& avalue, String::Language alang) {
		WContext::write(avalue, alang);
	}

public: // usage

	VMethodFrame(
		//const String& aname,
		const Junction& ajunction/*info: always method-junction*/,
		VMethodFrame *acaller);

//	const String& name() { return fname; }
	VMethodFrame *caller() { return fcaller; }

	void set_self(Value& aself) { fself=&aself; }
	/// we sure that someone already set our self with VMethodFrame::set_self(Value&)
	Value& self() { return *fself; }

	bool can_store_param() {
		const Method& method=*junction.method;
		return method.params_names && store_param_index<method.params_names->count();
	}
	void store_param(Value& value) {
		const Method& method=*junction.method;
		size_t max_params=
			method.max_numbered_params_count?method.max_numbered_params_count:
			method.params_names?method.params_names->count():
			0;
		if(store_param_index==max_params)
			throw Exception("parser.runtime",
				0, //&name(),
				"method of %s (%s) accepts maximum %d parameter(s)", 
					junction.self.get_class()->name_cstr(),
					junction.self.type(),
					max_params);
		
		if(fnumbered_params) { // are this method params numbered?
			*fnumbered_params+=&value;
		} else { // named param
			// speedup: not checking for clash with "result" fname
			const String& fname=*(*method.params_names)[store_param_index];
			set_my_variable(fname, value);
		}
		store_param_index++;
	}
	void fill_unspecified_params() {
		const Method &method=*junction.method;
		if(method.params_names) { // there are any named parameters might need filling?
			size_t param_count=method.params_names->count();
			for(; store_param_index<param_count; store_param_index++) {
				const String& fname=*(*method.params_names)[store_param_index];
				my->put(fname, new VVoid);
			}
		}
	}

	MethodParams* numbered_params() { return fnumbered_params; }

private:

	void set_my_variable(const String& fname, Value& value) {
		my->put(fname, &value); // remember param
	}

	Value* get_result_variable();

public:
	
	const Junction& junction;

};

///	Auto-object used for temporary changing VMethod_frame::fself.
class Temp_method_frame_self {
	VMethodFrame& fmethod_frame;
	Value* saved_self;
public:
	Temp_method_frame_self(VMethodFrame& amethod_frame, Value& aself) :
		fmethod_frame(amethod_frame),
		saved_self(&amethod_frame.self()) {
		fmethod_frame.set_self(aself);
	}
	~Temp_method_frame_self() {
		fmethod_frame.set_self(*saved_self);
	}
};

#endif
