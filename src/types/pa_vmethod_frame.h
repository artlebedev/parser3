/** @file
	Parser: @b method_frame write context

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_VMETHOD_FRAME_H
#define PA_VMETHOD_FRAME_H

#define IDENT_PA_VMETHOD_FRAME_H "$Id: pa_vmethod_frame.h,v 1.136 2024/12/07 13:59:29 moko Exp $"

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
	MethodParams() : felements(0), fsize(0) {}

#ifdef USE_DESTRUCTORS
	~MethodParams() {
		Value **flast=felements+count();
		for(Value **current=felements;current<flast;current++){
			Junction* junction=(*current)->get_junction();
			if (junction && junction->code)
				delete (VJunction*)(*current);
		}
	}
#endif

	void store_params(Value **params, size_t count) {
		felements=params;
		fsize=count;
	}

	inline size_t count() const { return fsize; }

	inline Value& get(size_t index) const {
		assert(index<count());
		return *felements[index];
	}

	inline Value& operator[] (size_t index) { return get(index); }

	/// handy is-value-a-junction ensurer
	Value& as_junction(int index, const char* msg) {
		Value& value=get(index);
		if(value.get_junction())
			return value;
		throw Exception(PARSER_RUNTIME, 0, "%s (parameter #%d)", msg, 1+index);
	}

	/// handy value-is-not-a-junction ensurer
	Value& as_no_junction(int index, const char* msg) {
		Value& value=get(index);
		if(!value.get_junction())
			return value;
		throw Exception(PARSER_RUNTIME, 0, "%s (parameter #%d)", msg, 1+index);
	}

	/// handy is-value-a-junction ensurer or can be auto-processed
	Value& as_expression(int index, const char* msg) {
		Value& value=get(index);
		if(value.is_evaluated_expr())
			return value;
		if(value.get_junction())
			return value;
		throw Exception(PARSER_RUNTIME, 0, "%s (parameter #%d)", msg, 1+index);
	}

	/// handy expression auto-processing to double
	double as_double(int index, const char* msg, Request& r) {
		Value& value=get(index);
		if(value.is_evaluated_expr())
			return value.as_double();
		return get_processed(value, msg, index, r).as_double();
	}

	/// handy expression auto-processing to int
	int as_int(int index, const char* msg, Request& r) {
		Value& value=get(index);
		if(value.is_evaluated_expr())
			return value.as_int();
		return get_processed(value, msg, index, r).as_int();
	}

	/// handy param auto-processing to index
	int as_index(int index, size_t count, Request& r) {
		if(get(index).is_string()) {
			const String& svalue=*get(index).get_string();
			if(svalue == "last")
				return count-1;
			else if(svalue != "first")
				throw Exception(PARSER_RUNTIME, &svalue, "index must be 'first', 'last' or expression");
			return 0;
		} else {
			int result=as_int(index, "index must be 'first', 'last' or expression", r);
			if(result < 0)
				result+=count;
			return result;
		}
	}

	/// handy expression auto-processing to bool
	bool as_bool(int index, const char* msg, Request& r) {
		Value& value=get(index);
		if(value.is_evaluated_expr())
			return value.as_bool();
		return get_processed(value, msg, index, r).as_bool();
	}

	/// handy string ensurer
	const String& as_string(int index, const char* msg) {
		return as_no_junction(index, msg).as_string();
	}

	/// handy file name ensurer
	const String& as_file_name(int index) {
		const String& result=as_no_junction(index, FILE_NAME_MUST_BE_NE_STRING).as_string();
		if(result.is_empty())
			throw Exception(PARSER_RUNTIME, 0, "%s (parameter #%d)", FILE_NAME_MUST_BE_NE_STRING, 1+index);
		return result;
	}

	/// handy hash ensurers
	HashStringValue* as_hash(int index, const char* name=0);

	/// handy table ensurer
	Table* as_table(int index, const char* name=0);

private:

	Value **felements;
	size_t fsize;

	Value& get_processed(Value& value, const char* msg, int index, Request& r);

};

/**
	Method frame write context
	accepts values written by method code
*/

class VMethodFrame: public WContext {
protected:
	VMethodFrame *fcaller;
	Value& fself;

public: // Value

	override const char* type() const { return "method_frame"; }

	/// VMethodFrame: self_transparent
	override VStateless_class* get_class() { return self().get_class(); }

	/// VMethodFrame: self_transparent
	override VStateless_class* base() { return self().base(); }

public: // usage

	VMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself):
		WContext(0 /* no parent, junctions can be reattached only up to VMethodFrame */),
		fcaller(acaller),
		fself(aself),
		method(amethod) {
	}

	VMethodFrame *caller() { return fcaller; }

	Value& self() { return fself; }

	void check_call_type(){
		if(method.call_type==Method::CT_ANY)
			return;

		Method::Call_type call_type=self().get_class()==&self() ? Method::CT_STATIC : Method::CT_DYNAMIC;

		 if(method.call_type!=call_type) // call type not allowed?
			throw Exception(PARSER_RUNTIME, method.name, "method of '%s' is not allowed to be called %s", 
				self().type(), call_type==Method::CT_STATIC ? "statically" : "dynamically");
	}

public:

	const Method& method;
};


class VNativeMethodFrame: public VMethodFrame {
protected:
	MethodParams fnumbered_params;

public: // usage

	VNativeMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : VMethodFrame(amethod, acaller, aself){}

	// params should be declared outside of *_FRAME_ACTION as MethodParams destructor will be called in ~VNativeMethodFrame
	void store_params(Value **params, size_t count) {
		fnumbered_params.store_params(params, count);
		method.check_actual_numbered_params(self(), &fnumbered_params);
	}

	void empty_params() {
		method.check_actual_numbered_params(self(), &fnumbered_params);
	}

	void call(Request &r);

};


/**
	Handles named parameters and local variables
*/

class VParserMethodFrame: public VMethodFrame {
public: // Value

	/// VParserMethodFrame: $result | parent get_string(=accumulated fstring)
	override const String* get_string() {
		// check the $result value
		Value* result=get_result_variable();
		// if we have one, return it's string value, else return as usual: accumulated fstring or fvalue
		return result ? result->get_string() : WContext::get_string();
	}

	/// VParserMethodFrame: my or self_transparent or $caller
	override Value* get_element(const String& aname) {
		if(SYMBOLS_EQ(aname,CALLER_SYMBOL))
			return get_caller_wrapper();

		if(SYMBOLS_EQ(aname,SELF_SYMBOL))
			return &self();

		if(Value* result=my.get(aname))
			return result;

		if(Value* result=self().get_element(aname))
			return result;

		return 0;
	}

	/// VParserMethodFrame: my or self_transparent
	override const VJunction* put_element(const String& aname, Value* avalue) {
		if(my.put_replaced(aname, avalue))
			return 0;
		return self().put_element(aname, avalue);
	}

public: // WContext

	/// VParserMethodFrame: skip write when RO_USE_RESULT
	override void write(const String& astring) {
#ifdef OPTIMIZE_RESULT
		if(method.result_optimization != Method::RO_USE_RESULT)
#endif
			WContext::write(astring);
	}

	override void write(Value& avalue) {
		WContext::write(avalue);
	}

	override ValueRef result() {
		// check the $result value
		Value* result_value=get_result_variable();
		// if we have one, return it, else return as usual: accumulated fstring or fvalue
		if(result_value){
#ifdef OPTIMIZE_RESULT
			((Method *)&method)->result_optimization=Method::RO_USE_RESULT;
#endif
			return result_value;
		}
#ifdef OPTIMIZE_RESULT
		if(method.result_optimization==Method::RO_USE_RESULT)
			return VVoid::get();
		((Method *)&method)->result_optimization=Method::RO_USE_WCONTEXT;
#ifdef OPTIMIZE_CALL // nested as CO_WITHOUT_WCONTEXT assumes that $result not used
		((Method *)&method)->call_optimization=Method::CO_WITHOUT_WCONTEXT;
#endif
#endif
		return WContext::result();
	}

public: // usage

	HashString<Value*> my; // public for ^stack[]

	VParserMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself);

	void store_params(Value **params, size_t count) {
		size_t param_count=method.params_count;
		size_t i=0;

		if(count>param_count){

			for(; i<param_count; i++) {
				const String& fname=*(*method.params_names)[i];
				my.put(fname, params[i]);
			}

			if(method.extra_params){
				VHash& vargs=*new VHash();
				HashStringValue& args = vargs.hash();

				for(; i<count; i++) {
					args.put(pa_uitoa(args.count()), params[i]);
				}

				my.put(*method.extra_params, &vargs);
			} else if(method.named_params){
				if(count!=param_count+1)
					throw Exception(PARSER_RUNTIME, method.name, "method of '%s' accepts maximum %d parameter(s) (%d present)", self().type(), param_count+1, count);

				HashStringValue* named_args = params[i]->as_hash("named parameter");
				size_t named_count=method.named_params->count();
 				for(i=0; i<named_count; i++) {
					const String& fname=*(*method.named_params)[i];
					Value *arg=named_args ? named_args->get(fname) : NULL;
					my.put(fname, arg ? arg : VVoid::get());
				}
			} else
				throw Exception(PARSER_RUNTIME, method.name, "method of '%s' accepts maximum %d parameter(s) (%d present)", self().type(), param_count, count);
		} else {

			for(; i<count; i++) {
				const String& fname=*(*method.params_names)[i];
				my.put(fname, params[i]);
			}

			for(; i<param_count; i++) {
				const String& fname=*(*method.params_names)[i];
				my.put(fname, VVoid::get());
			}

			if(method.extra_params){
				my.put(*method.extra_params, VVoid::get());
			} else if(method.named_params){
				size_t named_count=method.named_params->count();
				for(i=0; i<named_count; i++) {
					const String& fname=*(*method.named_params)[i];
					my.put(fname, VVoid::get());
				}
			}
		}
	}

	void empty_params(){
		size_t param_count=method.params_count;
		if(param_count>0){
			my.put(*(*method.params_names)[0], VString::empty());
			for(size_t i=1; i<param_count; i++)
				my.put(*(*method.params_names)[i], VVoid::get());
		}
	}

	void call(Request &r);

protected:

	void set_my_variable(const String& fname, Value& value) {
		my.put(fname, &value); // remember param
	}

	Value* get_result_variable();

	Value* get_caller_wrapper();

};


class VLocalParserMethodFrame: public VParserMethodFrame {
public: // Value

	override const VJunction* put_element(const String& aname, Value* avalue){
		set_my_variable(aname, *avalue);
		return 0;
	}

public: // usage

	VLocalParserMethodFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : VParserMethodFrame(amethod, acaller, aself) {}

};


template<typename Parent> class VExpressionFrame: public Parent {
public:
	VExpressionFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : Parent(amethod, acaller, aself) {}

	/// in expressions only strings are written as strings
	override void write_as_string(Value& avalue) {
		if(avalue.is_string())
			Parent::write(*avalue.get_string());
		else
			Parent::write(avalue);
	}
};


template<typename Parent> class VConstructorFrame: public Parent {
public:
	VConstructorFrame(const Method& amethod, VMethodFrame *acaller, Value& aself) : Parent(amethod, acaller, aself) {
		// prevent non-string writes for better error reporting [constructors are not expected to return anything]
		VMethodFrame::write(aself);
	}

	override void write(const String& /*astring*/) {}
};


#define METHOD_FRAME_ACTION(method, caller, self, action)					\
	if((method).native_code){								\
		VNativeMethodFrame frame(method, caller, self);					\
		action;										\
	} else {										\
		if((method).all_vars_local){							\
			VLocalParserMethodFrame frame(method, caller, self);			\
			action;									\
		} else {									\
			VParserMethodFrame frame(method, caller, self);				\
			action;									\
		}										\
	}

#define EXPRESSION_FRAME_ACTION(method, caller, self, action)					\
	if((method).native_code){								\
		VExpressionFrame<VNativeMethodFrame> frame(method, caller, self);		\
		action;										\
	} else {										\
		if((method).all_vars_local){							\
			VLocalParserMethodFrame frame(method, caller, self);			\
			action;									\
		} else {									\
			VParserMethodFrame frame(method, caller, self);				\
			action;									\
		}										\
	}

#define CONSTRUCTOR_FRAME_ACTION(method, caller, self, action)					\
	if((method).native_code){								\
		VConstructorFrame<VNativeMethodFrame> frame(method, caller, self);		\
		action;										\
	} else {										\
		if((method).all_vars_local){							\
			VConstructorFrame<VLocalParserMethodFrame> frame(method, caller, self);	\
			action;									\
		} else {									\
			VConstructorFrame<VParserMethodFrame> frame(method, caller, self);	\
			action;									\
		}										\
	}

#endif
