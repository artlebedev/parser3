/** @file
	Parser: Method class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_METHOD_H
#define PA_METHOD_H

#define IDENT_PA_METHOD_H "$Id: pa_method.h,v 1.32 2020/12/15 17:10:38 moko Exp $"

#define OPTIMIZE_CALL
#define OPTIMIZE_RESULT

#include "pa_operation.h"
#include "pa_vjunction.h"

/**
	native code method
	params can be NULL when 
	method min&max params (see VStateless_class::add_native_method)
	counts are zero.	
*/
typedef void (*NativeCodePtr)(Request& request, MethodParams& params);


/** 
	class method.

	methods can have 
	- named or
	- numbered parameters

	methods can be
	- parser or 
	- native onces

	holds
	- parameter names or number limits
	- local names
	- code [parser or native]
*/
class Method: public PA_Object {
public:

	/// allowed method call types
	enum Call_type {
		CT_ANY, ///< method can be called either statically or dynamically
		CT_STATIC, ///< method can be called only statically
		CT_DYNAMIC ///< method can be called only dynamically
	};
	
	enum Result_optimization {
		RO_UNKNOWN,
		RO_USE_RESULT, // write to $result detected, will not collect all writes to output scope.
		RO_USE_WCONTEXT // native code or parser code without $result usage.
	};

	enum Call_optimization {
		CO_NONE,
		CO_WITHOUT_FRAME, // for some native methods method frame is not required, faster but $self is unavailable
		CO_WITHOUT_WCONTEXT // for all native methods wcontext is not required, as no $result, faster
	};

	Call_type call_type;

	/// either numbered params for native-code methods = operators
	int min_numbered_params_count, max_numbered_params_count;

	/// or named params&locals for parser-code methods
	ArrayString* params_names;
	int params_count;
	ArrayString* locals_names;

	/// the Code
	ArrayOperation* parser_code; /*OR*/ NativeCodePtr native_code;

	bool all_vars_local; // in local vars list 'locals' was specified: all vars are local

#ifdef OPTIMIZE_RESULT
	Result_optimization result_optimization;
#endif

#ifdef OPTIMIZE_CALL
	Call_optimization call_optimization;
#endif

	mutable VJunction *junction_template;

	const String *name; // method name, never null
	const String *extra_params; // method has *name as an argument

	Method(
		Call_type acall_type,
		int amin_numbered_params_count, int amax_numbered_params_count,
		ArrayString* aparams_names, ArrayString* alocals_names,
		ArrayOperation* aparser_code, NativeCodePtr anative_code,
		bool aall_vars_local=false
#ifdef OPTIMIZE_RESULT
		, Result_optimization aresult_optimization=RO_UNKNOWN
#endif
#ifdef OPTIMIZE_CALL
		, Call_optimization acall_optimization=CO_NONE
#endif
		) :

		call_type(acall_type),
		min_numbered_params_count(amin_numbered_params_count), max_numbered_params_count(amax_numbered_params_count),
		params_names(aparams_names), params_count(0), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code),
		all_vars_local(aall_vars_local),
#ifdef OPTIMIZE_RESULT
		result_optimization(aresult_optimization),
#endif
#ifdef OPTIMIZE_CALL
		call_optimization(acall_optimization),
#endif
		junction_template(0),
		name(&String::Empty) {
			if (params_names){
				params_count=(int)params_names->count();
				const char *last_param = params_names->get(params_count-1)->cstr();
				if (last_param[0] == '*' && last_param[1]){
					extra_params = new String(pa_strdup(last_param+1));
					params_names->remove(--params_count);
					return;
				}
			}
			extra_params = NULL;
	}

	/// call this before invoking to ensure proper actual numbered params count
	void check_actual_numbered_params(Value& self, MethodParams* actual_numbered_params) const;

	VJunction* get_vjunction(Value& aself) const {
		if(!junction_template)
			return junction_template=new VJunction(aself, this);
		return junction_template->get(aself);
	}
};

#endif
