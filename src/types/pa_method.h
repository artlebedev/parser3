/** @file
	Parser: Method class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_METHOD_H
#define PA_METHOD_H

static const char * const IDENT_METHOD_H="$Date: 2008/05/30 12:22:54 $";


/*#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
*/
#include "pa_operation.h"

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
	
	/// name for error reporting
	//const String& name;
	///
	Call_type call_type;
	//@{
	/// @name either numbered params // for native-code methods = operators
	int min_numbered_params_count, max_numbered_params_count;
	//@}
	//@{
	/// @name or named params&locals // for parser-code methods
	ArrayString* params_names;  ArrayString* locals_names;
	//@}
	//@{
	/// @name the Code
	ArrayOperation* parser_code;/*OR*/NativeCodePtr native_code;
	//@}
	bool all_vars_local;

	Method(
		//const String& aname,
		Call_type call_type,
		int amin_numbered_params_count, int amax_numbered_params_count,
		ArrayString* aparams_names, ArrayString* alocals_names,
		ArrayOperation* aparser_code, NativeCodePtr anative_code,
		bool aall_vars_local=false) : 

		//name(aname),
		call_type(call_type),
		min_numbered_params_count(amin_numbered_params_count),
		max_numbered_params_count(amax_numbered_params_count),
		params_names(aparams_names), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code),
		all_vars_local(aall_vars_local) {
	}

	/// call this before invoking to ensure proper actual numbered params count
	void check_actual_numbered_params(
		Value& self, /*const String& actual_name, */MethodParams* actual_numbered_params) const;
};

///	Auto-object used for temporarily substituting/removing elements
class Temp_value_element {
	Value& fwhere;
	const String& fname;
	Value* saved;
public:
	Temp_value_element(Value& awhere, const String& aname, Value* awhat) : 
		fwhere(awhere),
		fname(aname),
		saved(awhere.get_element(aname, awhere, false)) {
		fwhere.put_element(fwhere, aname, awhat, false);
	}
	~Temp_value_element() { 
		fwhere.put_element(fwhere, fname, saved, false);
	}
};


#endif
