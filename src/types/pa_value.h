/** @file
	Parser: Value, Method, Junction class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_value.h,v 1.25 2001/03/19 19:17:46 paf Exp $
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
class VTable;
class Junction;
class Method;
class Hash;

///	grandfather of all \a values in \b Parser
class Value : public Pooled {
public: // Value

	/// - all: for error reporting after fail(), etc
	virtual const char *type() const =0;
	/// - all: for error reporting after fail(), etc
	const String& name() const { return *fname; }
	/** is this value defined?
		@return for
		- VUnknown: false
		- others: true
	*/
	virtual bool get_defined() { return true; }
	/** what's the meaning of this value in context of expression?
		@return for
		- VString: fstring as VDouble
		- VBool: this
		- VDouble: this
		- VInt: this
		- VUnknown: this
	*/
	virtual Value *get_expr_result() { bark("(%s) can not be used in expression"); return 0; }
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
		- VInt: finteger
		- VBool: value
	*/
	virtual double get_double() { bark("(%s) does not have numerical value"); return 0; }
	/** extract bool
		@return for
		- VUnknown: false
		- VBool: value
		- VInt: 0 or !0
		- VDouble: 0 or !0
	*/
	virtual bool get_bool() { bark("(%s) does not have logical value"); return 0; }
	/** extract Junction
		@return for
		- junction: itself
	*/
	virtual Junction *get_junction() { return 0; }
	/** extract VTable
		@return for
		- VTable: itself
	*/
	virtual VTable *get_vtable() { return 0; }
	/** extract Value element
		@return for
		- VHash: (key)=value
		- object_class: (field)=STATIC.value;(STATIC)=hash;(method)=method_ref with self=object_class
		- object_base: (CLASS)=vclass;(BASE)=base;(method)=method_ref
		- object_instance: (field)=value;(CLASS)=vclass;(method)=method_ref
		- operator_class: (field)=value - static values only
		- VCode_frame: wcontext_transparent
		- methodframe: my or self_transparent
		- VTable: column
		- env: CLASS,BASE,method,field
		- VForm: CLASS,BASE,method,field
		- VString: $CLASS,$BASE,$method
		- VRequest: CLASS,BASE,method,fields
		- VResponse: CLASS,BASE,method,fields
		- VCookie: CLASS,BASE,method,field
	*/
	virtual Value *get_element(const String& name) { bark("(%s) does not have elements"); return 0; }
	/** store Value element under \a name
		@return for
		- VHash: (key)=value
		- VStateless_object: (CLASS)=vclass;(BASE)=base;(method)=method_ref
		- object_class: (field)=value - static values only
		- object_instance: (field)=value
		- VCode_frame: wcontext_transparent
		- methodframe: my or self_transparent
		- VResponse: (attribute)=value
		- VCookie: field
	*/
	virtual void put_element(const String& name, Value *value) { bark("(%s) does not accept elements"); }
	/** extract VStateless_class
		@return for
		- VStateless_class: this
		- VStateless_object: fclass_real
		- wcontext: none yet | transparent
		- VForm: this
		- class: this
		- env: this
		- VRequest: this
		- VHash: this
		- vVCookie: this
	*/
	virtual VStateless_class *get_class() { return 0; }
	/** extract VAliased
		@return for
		- valiased: this
		- wcontext: transparent
		- methodframe: self_transparent
	*/
	virtual VAliased *get_aliased() { return 0; }

public: // usage

	Value(Pool& apool) : Pooled(apool), fname(unnamed_name) {
	}

	/// set's the name which is used in error messages
	void set_name(const String& aname) { fname=&aname; }

	/// \return sure String. if it doesn't have string value barks
	const String& as_string() {
		const String *result=get_string(); 
		if(!result)
			bark("(%s) not a string");

		return *result;
	}

	/// \return sure VTable. if it doesn't have string value barks
	VTable& as_vtable() {
		VTable *result=get_vtable(); 
		if(!result)
			bark("(%s) not a table object");

		return *result;
	}

private:

	const String *fname;

protected: 

	/// throws exception specifying bark-reason and name() type() of problematic value
	void bark(char *reason) const {
		THROW(0,0,
			&name(),
			reason, type());
	}

};

/// native code method
typedef void (*Native_code_ptr)(Request& request, 
								const String& method_name, Array *params);

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
	class method.

	methods can have 
	- named or
	- numbered parameters

	methods can be
	- parser or 
	- native onces

	hold
	- parameter names or number limits
	- local names
	- code [parser or native]
*/
class Method : public Pooled {
public:
	/// method name for error reporting
	const String& name;
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
		int amin_numbered_params_count, int amax_numbered_params_count,
		Array *aparams_names, Array *alocals_names,
		const Array *aparser_code, Native_code_ptr anative_code) : 

		Pooled(apool),
		name(aname),
		min_numbered_params_count(amin_numbered_params_count),
		max_numbered_params_count(amax_numbered_params_count),
		params_names(aparams_names), locals_names(alocals_names),
		parser_code(aparser_code), native_code(anative_code) {
	}

	/// call this before invoking to ensure proper actual numbered params count
	void check_actual_numbered_params(
		Value& self, const String& actual_name, Array *actual_numbered_params) const {
		int actual_count=actual_numbered_params?actual_numbered_params->size():0;
		if(actual_count<min_numbered_params_count) // not proper count? bark
			THROW(0, 0,
				&actual_name,
				"native method of %s (%s) accepts minimum %d parameter(s)", 
					self.name().cstr(),
					self.type(),
					min_numbered_params_count);

	}
};

#endif
