/** @file
	Parser: Value, Method, Junction class decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

static const char* IDENT_VALUE_H="$Date: 2002/09/17 15:53:46 $";

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_globals.h"

// forwards

class VStateless_class;
class WContext;
class Request;
class Table;
class Junction;
class Method;
class Hash;
class VFile;
class MethodParams;
class VObject;
class VMethodFrame;

///	grandfather of all @a values in @b Parser
class Value : public Pooled {
public: // Value

	/// all: value type, used for error reporting and 'is' expression operator
	virtual const char *type() const =0;

	/** remember derived class instance 
	    - VObject: the only client
	*/
	virtual VObject *set_derived(VObject * /*aderived*/) { return 0; }

	/**
		all except VObject/VClass: this if @atype eq type()
		VObject/VClass: can locate parent class by it's type
	*/
	virtual Value *as(const char *atype, bool looking_up) {
		return atype && strcmp(type(), atype)==0?this:0;
	}
	/// type checking helper, uses Value::as
	bool is(const char *atype) { return as(atype, false)!=0; }
	
	/** is this value defined?
		@return for
		- VVoid: false
		- VString: eq ''=false, ne ''=true
		- VHash: count!=0
		- VTable: count!=0
		- others: true
	*/
	virtual bool is_defined() const { return true; }
	
	/** is this value string?
		@return for
		- VString: true
		- others: false
	*/
	virtual bool is_string() const { return false; }
	
	/** what's the meaning of this value in context of expression?
		@return for
		- VString: fstring as VDouble or this depending on return_string_as_is
		- VBool: clone
		- VDouble: clone
		- VInt: clone
		- VVoid: this
		- VFile: this
		- VImage: this
		- VDate: ftime -> float days
		- VTable: count
		- VHash: count
	*/
	virtual Value *as_expr_result(bool /*return_string_as_is*/=false) { 
		bark("(%s) can not be used in expression"); return 0; 
	}
	
	/** extract Hash
		@return for
		- VHash: fhash
		- VResponse: ffields
	*/
	virtual Hash *get_hash(const String * /*source*/) { return 0; }
	
	/** extract const String
		@return for
		- VString: value
		- VVoid: ""
		- VDouble: value
		- VInt: value
		- VBool: must be 0: so in ^if(1>2) it would'nt become "FALSE" string which is 'true'
		- others: 0
		- WContext: accumulated fstring
	*/
	virtual const String *get_string() { return 0; }
	
	/** extract double
		@return for
		- VString: value
		- VDouble: value
		- VInt: value
		- VBool: value
		- VVoid: 0
		- VDate: ftime -> float days
	*/
	virtual double as_double() const { bark("(%s) does not have numerical (double) value"); return 0; }
	
	/**	extract integer
		- VString: value
		- VDouble: value
		- VInt: value
		- VBool: value
		- VVoid: 0
		- VTable: count
		- VHash: count
	*/
	virtual int as_int () const { bark("(%s) does not have numerical (int) value"); return 0; }

	/** extract bool
		@return for
		- VVoid: false
		- VBool: value
		- VInt: 0 or !0
		- VDouble: 0 or !0
		- VFile: true
		- VDate: 0 or !0
		- VTable: count
		- VHash: count
	*/
	virtual bool as_bool() const { bark("(%s) does not have logical value"); return 0; }
	
	/** extract file
		@return for
		- VFile: this
		- VString: vfile
		- VImage: true
	*/
	virtual VFile *as_vfile(String::Untaint_lang /*lang*/=String::UL_UNSPECIFIED,
		bool /*origins_mode*/=false) { 
		bark("(%s) does not have file value"); return 0; 
	}
	
	/** extract Junction
		@return for
		- junction: itself
	*/
	virtual Junction *get_junction() { return 0; }
	
	/** extract base object of Value
		@return for
		- VObject: fbase
	*/
	virtual Value *base_object() { bark("(%s) has no base object"); return 0; }
	
	/** extract Value element
		@a self =0 means =this
		@return for
		- VHash: (key)=value
		- VStateless_class: +$method
		- VStateless_object: +$method
		- VClass: (field)=STATIC value;(method)=method_ref with self=object_class
		- VCodeFrame: wcontext_transparent
		- VMethodFrame: my or self_transparent
		- VTable: columns,methods
		- VEnv: field
		- VForm: CLASS,method,field
		- VString: $method
		- VRequest: fields
		- VResponse: method,fields
		- VCookie: field
		- VFile: method,field
		- VDate: CLASS,method,field
		*/
	virtual Value *get_element(const String& /*aname*/, Value * /*aself*/, bool /*looking_up*/) { bark("(%s) has no elements"); return 0; }

	/** store Value element under @a name
		@return for
		- VHash: (key)=value
		- VStateless_object: (CLASS)=vclass;(method)=method_ref
		- VStateless_class: (field)=value - static values only
		- VStateless_object: (field)=value
		- VCodeFrame: wcontext_transparent
		- VMethodFrame: my or self_transparent
		- VResponse: (attribute)=value
		- VCookie: field
	*/
	virtual bool put_element(const String& name, Value * /*value*/, bool /*replace*/) { 
		// to prevent modification of system classes,
		// created at system startup, and not having exception
		// handler installed, we neet to bark using request.pool
		bark("(%s) does not accept elements", 
			"element can not be stored to %s", &name); 
		return false;
	}
	
	/** extract VStateless_class
		@return for
		- VX: x_class
		- VStateless_class: this
		- VObject: fclass
		- WContext: none yet | transparent
		these are methodless classes:
		- VBool: 0
		- VJunction: 0
		- VEnv: 0
		- VRequest: 0
		- VCookie: 0
	*/
	virtual VStateless_class *get_class()=0;

	/** extract VTable
		@return for
		- VTable: ftable
		- VObject: from possible 'table' parent
	*/
	virtual Table *get_table() { return 0; }

public: // usage

	Value(Pool& apool) : Pooled(apool) {
	}

	/// @return sure String. if it doesn't have string value barks
	const String& as_string() {
		const String *result=get_string(); 
		if(!result)
			bark("(%s) has no string representation");

		return *result;
	}

protected: 

	/// throws exception specifying bark-reason and name() type() of problematic value
	void bark(char *reason, 
		const char *alt_reason=0, const String *problem_source=0) const {
		throw Exception("parser.runtime",
			problem_source,
			alt_reason?alt_reason:reason, type());
	}

};

/** \b junction is some code joined with context of it's evaluation.

	there are code-junctions and method-junctions
	- code-junctions are used when some parameter passed in cury brackets
	- method-junctions used in ^method[] calls or $method references

	Junctions register themselves in method_frame [if any] for consequent invalidation.
	This prevents evaluation of junctions in outdated context

	To stop situations like this:
@code
	@main[]
	^method1[]
	^method2[]

	@method1[]
	$junction{
		some code
	}

	@method2[]
	^junction[]
@endcode

	On wcontext[most dynamic context of all] scope exit (WContext::~WContext()) got cleaned - 
	Junction::wcontext becomes WContext.fparent (if any), 
	or Junction::method_frame becomes 0 (if no parent), which later in Request::process triggers exception

	parent changing helps ^switch implementation to remain simple
*/
class Junction : public Pooled {
public:

	Junction(Pool& apool,
		Value *aself,
		const Method *amethod,
		VMethodFrame *amethod_frame,
		Value *arcontext,
		WContext *awcontext,
		const Array *acode);

	void reattach(WContext *new_wcontext);

	/// present when not operator [selfless]
	Value *self;
	//@{
	/// @name either these // so called 'method-junction'
	const Method *method;
	//@}
	//@{
	/// @name or these are present // so called 'code-junction'
	VMethodFrame *method_frame;
	Value *rcontext;
	WContext *wcontext;
	const Array *code;
	//@}
};

/**
	native code method
	params can be NULL when 
	method min&max params (see VStateless_class::add_native_method)
	counts are zero.	
*/
typedef void (*Native_code_ptr)(Request& request, 
								const String& method_name, 
								MethodParams *params);

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
class Method : public Pooled {
public:

	/// allowed method call types
	enum Call_type {
		CT_ANY, ///< method can be called either statically or dynamically
		CT_STATIC, ///< method can be called only statically
		CT_DYNAMIC ///< method can be called only dynamically
	};
	
	/// name for error reporting
	const String& name;
	///
	Call_type call_type;
	//@{
	/// @name either numbered params // for native-code methods = operators
	int min_numbered_params_count, max_numbered_params_count;
	//@}
	//@{
	/// @name or named params&locals // for parser-code methods
	Array *params_names;  Array *locals_names;
	//@}
	//@{
	/// @name the Code
	const Array *parser_code;/*OR*/Native_code_ptr native_code;
	//@}

	Method(
		Pool& apool,
		const String& aname,
		Call_type call_type,
		int amin_numbered_params_count, int amax_numbered_params_count,
		Array *aparams_names, Array *alocals_names,
		const Array *aparser_code, Native_code_ptr anative_code) : 

		Pooled(apool),
		name(aname),
		call_type(call_type),
		min_numbered_params_count(amin_numbered_params_count),
		max_numbered_params_count(amax_numbered_params_count),
		params_names(aparams_names), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code) {
	}

	/// call this before invoking to ensure proper actual numbered params count
	void check_actual_numbered_params(
		Value *self, const String& actual_name, Array *actual_numbered_params) const;
};

///	Auto-object used for temporarily substituting/removing elements
class Temp_value_element {
	Value& fwhere;
	const String& fname;
	Value *saved;
public:
	Temp_value_element(Value& awhere, const String& aname, Value *awhat) : 
		fwhere(awhere),
		fname(aname),
		saved(awhere.get_element(aname, &awhere, false)) {
		fwhere.put_element(aname, awhat, false);
	}
	~Temp_value_element() { 
		fwhere.put_element(fname, saved, false);
	}
};

#endif
