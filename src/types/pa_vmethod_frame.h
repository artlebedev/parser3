/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001-2015 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VMETHOD_FRAME_H
#define PA_VMETHOD_FRAME_H

#define IDENT_PA_VMETHOD_FRAME_H "$Id: pa_vmethod_frame.h,v 1.102 2016/05/24 14:28:24 moko Exp $"

#include "pa_symbols.h"
#include "pa_wcontext.h"
#include "pa_vvoid.h"
#include "pa_vjunction.h"

// forwards

class Request;

/**
	@b method parameters passed in this array.
	contains handy typecast ad junction/not junction ensurers

*/
class MethodParams {
public:
	MethodParams() : felements(0), fused(0){}

#ifdef USE_DESTRUCTORS
	~MethodParams(){
		Value **flast=felements+count();
		for(Value **current=felements;current<flast;current++){
			Junction* junction=(*current)->get_junction();
			if (junction && junction->code)
				delete *current;
		}
	}
#endif

	void store_params(Value **params, size_t count){ 
		felements=params;
		fused=count;
	}

	inline size_t count() const { return fused; }

	inline Value *get(size_t index) const {
		assert(index<count());
		return felements[index];
	}

	inline Value& operator[] (size_t index) { return *get(index); }

	Value& last() { return *get(count()-1); }

	/// handy is-value-a-junction ensurer
	Value& as_junction(int index, const char* msg) { 
		Value* value=get(index);
		return as_junction(value, msg, index); 
	}
	/// handy is-value-a-junction ensurer
	Value& as_junction(Value* value, const char* msg, int index) { 
		return get_as(value, true, msg, index); 
	}
	/// handy value-is-not-a-junction ensurer
	Value& as_no_junction(int index, const char* msg) { 
		Value* value=get(index);
		return as_no_junction(value, msg, index); 
	}
	/// handy value-is-not-a-junction ensurer
	Value& as_no_junction(Value* value, const char* msg, int index) { 
		return get_as(value, false, msg, index); 
	}
	/// handy is-value-a-junction ensurer or can be auto-processed
	Value& as_expression(int index, const char* msg) { 
		Value* value=get(index);
		if(value->is_evaluated_expr()){
			return *value;
		} else {
			return get_as(value, true, msg, index);
		}
	}
	/// handy expression auto-processing to double
	double as_double(int index, const char* msg, Request& r) { 
		Value* value=get(index);
		if(!value->is_evaluated_expr())
			value=&get_processed(value, msg, index, r);
		return value->as_double(); 
	}
	/// handy expression auto-processing to int
	int as_int(int index, const char* msg, Request& r) { 
		Value* value=get(index);
		if(!value->is_evaluated_expr())
			value=&get_processed(value, msg, index, r);
		return value->as_int(); 
	}
	/// handy expression auto-processing to bool
	bool as_bool(int index, const char* msg, Request& r) { 
		Value* value=get(index);
		if(!value->is_evaluated_expr())
			value=&get_processed(value, msg, index, r);
		return value->as_bool(); 
	}
	/// handy string ensurer
	const String& as_string(int index, const char* msg) { 
		return as_no_junction(index, msg).as_string();
	}
	/// handy hash ensurers
	HashStringValue* as_hash(int index, const char* name=0);
	/// handy table ensurer
	Table* as_table(int index, const char* name=0);

private:

	Value **felements;
	size_t fused;

	/// handy value-is/not-a-junction ensurer
	Value& get_as(Value* value, bool as_junction, const char* msg, int index) { 
		if((value->get_junction()!=0) ^ as_junction)
			throw Exception(PARSER_RUNTIME,
				0,
				"%s (parameter #%d)", msg, 1+index);

		return *value;
	}

	Value& get_processed(Value* value, const char* msg, int index, Request& r);

};

/**	Method frame write context
	accepts values written by method code
	also handles method parameters and local variables
*/
class VMethodFrame: public WContext {
protected:
	VMethodFrame *fcaller;

	HashString<Value*>* my; /*OR*/ MethodParams fnumbered_params;
	Value& fself;

	typedef const VJunction* (VMethodFrame::*put_element_t)(const String& aname, Value* avalue);
	put_element_t put_element_impl;

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
	override Value* get_element(const String& aname) { 
		if(SYMBOLS_EQ(aname,Symbols::caller))
			return caller();

		if(SYMBOLS_EQ(aname,Symbols::self))
			return &self();

		Value* result;
		if(my && (result=my->get(aname)))
			return result;

		if(result=self().get_element(aname))
			return result;

		return 0;
	}

	/// VMethodFrame: self_transparent
	override VStateless_class* get_class() { return self().get_class(); }

	/// VMethodFrame: self_transparent
	override VStateless_class* base() { return self().base(); }

	/// VMethodFrame: my or self_transparent
	override const VJunction* put_element(const String& aname, Value* avalue) {
		return (this->*put_element_impl)(aname, avalue);
	}

	/// VMethodFrame: appends a fstring to result
	override void write(const String& astring, String::Language alang) {
#ifdef OPTIMIZE_RESULT
		switch (method.result_optimization){
			case Method::RO_USE_RESULT:
				return;
			case Method::RO_UNKNOWN:
				if(get_result_variable()){
					((Method *)&method)->result_optimization=Method::RO_USE_RESULT;
					return;
				}
			case Method::RO_USE_WCONTEXT: break;
		}
#endif
		WContext::write(astring, alang);
	}

private:

	const VJunction* put_element_local(const String& aname, Value* avalue){
		set_my_variable(aname, *avalue);
		return PUT_ELEMENT_REPLACED_ELEMENT;
	}

	const VJunction* put_element_global(const String& aname, Value* avalue){
		if(my && my->put_replaced(aname, avalue))
			return PUT_ELEMENT_REPLACED_ELEMENT;
		return self().put_element(aname, avalue);
	}

public: // WContext

	override StringOrValue result() {
		if(my){
			// check the $result value
			Value* result_value=get_result_variable();
			// if we have one, return it, else return as usual: accumulated fstring or fvalue
			if(result_value)
				return StringOrValue(*result_value);
#ifdef OPTIMIZE_RESULT
			if(method.result_optimization==Method::RO_USE_RESULT)
				return StringOrValue(*VVoid::get());
			((Method *)&method)->result_optimization=Method::RO_USE_WCONTEXT;
#ifdef OPTIMIZE_CALL // nested as CO_WITHOUT_WCONTEXT assumes that $result not used
			((Method *)&method)->call_optimization=Method::CO_WITHOUT_WCONTEXT;
#endif
#endif
		}
		return WContext::result();
	}

	override void write(Value& avalue) {
		WContext::write(avalue);
	}

public: // usage

	VMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself);

	VMethodFrame *caller() { return fcaller; }

#ifdef USE_DESTRUCTORS
	~VMethodFrame(){
		if(my){
			delete my;
		}
	}
#endif

	Value& self() { return fself; }

	size_t method_params_count() {
		return method.params_names ? method.params_names->count():0;
	}

	void store_params(Value **params, size_t count) {
		if(my) {
			size_t param_count=method.params_names ? method.params_names->count():0;
			size_t i=0;

			if(count>param_count){
				if(method.extra_params){
					for(; i<param_count; i++) {
						const String& fname=*(*method.params_names)[i];
						set_my_variable(fname, *params[i]);
					}

					VHash& vargs=*new VHash();
					HashStringValue& args = vargs.hash();

					for(; i<count; i++) {
						args.put(format(args.count(), 0), params[i]);
					}

					set_my_variable(*method.extra_params, vargs);
					return;
				} else
				throw Exception(PARSER_RUNTIME,
					0, //&name(),
					"method of %s accepts maximum %d parameter(s) (%d present)", 
					self().type(),
					param_count,
					count);
			}

			for(; i<count; i++) {
				const String& fname=*(*method.params_names)[i];
				set_my_variable(fname, *params[i]);
			}

			for(; i<param_count; i++) {
				const String& fname=*(*method.params_names)[i];
				my->put(fname, VVoid::get());
			}
		} else
			fnumbered_params.store_params(params,count);
	}

	void empty_params(){
		if(method.params_names){
			size_t param_count=method.params_names->count();
			if(param_count>0){
				const String& fname=*(*method.params_names)[0];
				my->put(fname, VString::empty());
				for(size_t i=1; i<param_count; i++) {
					const String& fname=*(*method.params_names)[i];
					my->put(fname, VVoid::get());
				}
			}
		}
	}

	MethodParams* numbered_params() { return &fnumbered_params; }

protected:

	void set_my_variable(const String& fname, Value& value) {
		my->put(fname, &value); // remember param
	}

	Value* get_result_variable();

public:
	
	const Method& method;

};

class VConstructorFrame: public VMethodFrame {
public:
	VConstructorFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : VMethodFrame(amethod, acaller, aself) {
		// prevent non-string writes for better error reporting [constructors are not expected to return anything]
		VMethodFrame::write(aself);
	}

	override void write(const String& /*astring*/, String::Language /*alang*/) {}
};

#endif
