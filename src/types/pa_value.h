/** @file
	Parser: Value, Method, Junction class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_value.h,v 1.60 2001/05/15 10:01:25 parser Exp $
*/

#ifndef PA_VALUE_H
#define PA_VALUE_H

#include "pa_pool.h"
#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_globals.h"

class VStateless_class;
class WContext;
class VAliased;
class Request;
class Table;
class Junction;
class Method;
class Hash;
class VFile;

///	grandfather of all @a values in @b Parser
class Value : public Pooled {
public: // Value

	/// all: value type, used for error reporting and 'is' expression operator
	virtual const char *type() const =0;
	
	const String& name() const { return *fname; }
	
	/** is this value defined?
		@return for
		- VUnknown: false
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
		- VBool: this
		- VDouble: this
		- VInt: this
		- VUnknown: this
		- VFile: this
		- VImage: this
	*/
	virtual Value *as_expr_result(bool return_string_as_is=false) { 
		bark("(%s) can not be used in expression"); return 0; 
	}
	
	/** extract Hash
		@return for
		- VHash: fhash
		- VResponse: ffields
	*/
	virtual Hash *get_hash() { return 0; }
	
	/** extract const String
		@return for
		- VString: value
		- VUnknown: ""
		- VDouble: value
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
		- VUnknown: 0
	*/
	virtual double as_double() { bark("(%s) does not have numerical (double) value"); return 0; }
	
	/**	extract integer
		- VString: value
		- VDouble: value
		- VInt: value
		- VBool: value
		- VUnknown: 0
	*/
	virtual int as_int () { bark("(%s) does not have numerical (int) value"); return 0; }

	/** extract bool
		@return for
		- VUnknown: false
		- VBool: value
		- VInt: 0 or !0
		- VDouble: 0 or !0
		- VFile: true
	*/
	virtual bool as_bool() { bark("(%s) does not have logical value"); return 0; }
	
	/** extract file
		@return for
		- VFile: this
		- VString: vfile
		- VImage: true
	*/
	virtual const VFile *as_vfile(String::Untaint_lang lang=String::UL_UNSPECIFIED) const { 
		bark("(%s) does not have file value"); return 0; 
	}
	
	/** extract Junction
		@return for
		- junction: itself
	*/
	virtual Junction *get_junction() { return 0; }
	
	/** extract Value element
		@return for
		- VHash: (key)=value
		- VAliased: sometimes $CLASS, $BASE [see VAliased::hide_class()]
		- VStateless_class: +$method
		- VStateless_object: +$method
		- VClass: (field)=STATIC value;(method)=method_ref with self=object_class
		- VCodeFrame: wcontext_transparent
		- VMethodFrame: my or self_transparent
		- VTable: columns,methods
		- VEnv: field
		- VForm: CLASS,BASE,method,field
		- VString: $method
		- VRequest: fields
		- VResponse: method,fields
		- VCookie: field
		- VFile: method,field
		*/
	virtual Value *get_element(const String& name) { bark("(%s) has no elements"); return 0; }
	
	/** store Value element under @a name
		@return for
		- VHash: (key)=value
		- VStateless_object: (CLASS)=vclass;(BASE)=base;(method)=method_ref
		- VStateless_class: (field)=value - static values only
		- VStateless_object: (field)=value
		- VCodeFrame: wcontext_transparent
		- VMethodFrame: my or self_transparent
		- VResponse: (attribute)=value
		- VCookie: field
	*/
	virtual void put_element(const String& name, Value *value) { bark("(%s) does not accept elements"); }
	
	/** extract VStateless_class
		@return for
		- VStateless_class: this
		- VStateless_object: fclass_real
		- WContext: none yet | transparent
		- VHash: 0
	*/
	virtual VStateless_class *get_class() { return 0; }
	
	/** extract VAliased
		@return for
		- VAliased: this
		- WContext: transparent
		- VMethodFrame: self_transparent
	*/
	virtual VAliased *get_aliased() { return 0; }

	/** extract VTable
		@return for
		- VTable: ftable
	*/
	virtual Table *get_table() { return 0; }

public: // usage

	Value(Pool& apool) : Pooled(apool), fname(unnamed_name) {
	}

	/// set's the name which is used in error messages
	void set_name(const String& aname) { fname=&aname; }

	/// @return sure String. if it doesn't have string value barks
	const String& as_string() {
		const String *result=get_string(); 
		if(!result)
			bark("(%s) has no string representation");

		return *result;
	}

private:

	const String *fname;

protected: 

	/// throws exception specifying bark-reason and name() type() of problematic value
	void bark(char *reason) const {
		THROW(0, 0,
			&name(),
			reason, type());
	}

};

/** \b junction is some code joined with context of it's evaluation.

	there are code-junctions and method-junctions
	- code-junctions are used when some parameter passed in cury brackets
	- method-junctions used in ^method[] calls or $method references
*/
class Junction : public Pooled {
public:

	Junction(Pool& apool,
		Value& aself,
		VStateless_class *avclass, const Method *amethod,
		Value *aroot,
		Value *arcontext,
		WContext *awcontext,
		const Array *acode) : Pooled(apool),
		
		self(aself),
		vclass(avclass), method(amethod),
		root(aroot),
		rcontext(arcontext),
		wcontext(awcontext),
		code(acode) {
	}

	/// always present
	Value& self;
	//@{
	/// @name either these // so called 'method-junction'
	VStateless_class *vclass;  const Method *method;
	//@}
	//@{
	/// @name or these are present // so called 'code-junction'
	Value *root;
	Value *rcontext;
	WContext *wcontext;
	const Array *code;
	//@}
};

/**
	@b method parameters passed in this array.
	contains handy typecast ad junction/not junction ensurers

*/
class MethodParams : public Array {
public:
	MethodParams(Pool& pool, const String& amethod_name) : Array(pool),
		fmethod_name(amethod_name) {
	}

	/// handy typecast. I long for templates
	Value& get(int index) { return *static_cast<Value *>(Array::get(index)); }
	/// handy is-value-a-junction ensurer
	Value& get_junction(int index, const char *msg) { return get_as(index, true, msg); }
	/// handy value-is-not-a-junction ensurer
	Value& get_no_junction(int index, const char *msg) { return get_as(index, false, msg); }

private:

	/// handy value-is/not-a-junction ensurer
	Value& get_as(int index, bool as_junction, const char *msg) { 
		Value& result=get(index);
		if((result.get_junction()!=0) ^ as_junction)
			THROW(0, 0,
				&fmethod_name,
				"%s (parameter #%d)", msg, index);
		return result;
	}

private:

	const String& fmethod_name;

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
	void check_actual_numbered_params(Pool& pool,
		Value& self, const String& actual_name, Array *actual_numbered_params) const {

		int actual_count=actual_numbered_params?actual_numbered_params->size():0;
		if(actual_count<min_numbered_params_count) // not proper count? bark
			PTHROW(0, 0,
				&actual_name,
				"native method of %s (%s) accepts minimum %d parameter(s) (%d present)", 
					self.name().cstr(),
					self.type(),
					min_numbered_params_count,
					actual_count);

	}
};

#endif
