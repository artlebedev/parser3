/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: root.C,v 1.42 2001/03/18 14:45:25 paf Exp $
*/

#include <string.h>
#include <math.h>

#include "pa_request.h"
#include "_root.h"
#include "pa_vint.h"
#include "pa_common.h"

static void _if(Request& r, const String& method_name, Array *params) {
	Value& condition_code=*static_cast<Value *>(params->get(0));
	// forcing ^if(this param type)
	r.fail_if_junction_(false, condition_code, 
		method_name, "condition must be junction");

	bool condition=r.process(condition_code, 
		0/*no name*/,
		false/*don't intercept string*/).get_bool();
	if(condition) {
		Value& then_code=*static_cast<Value *>(params->get(1));
		// forcing ^if(this param type)
		r.fail_if_junction_(false, then_code, 
			method_name, "then-parameter must be junction");
		r.write_pass_lang(r.process(then_code));
	} else if(params->size()==3) {
		Value& else_code=*static_cast<Value *>(params->get(2));
		// forcing ^if(this param type)
		r.fail_if_junction_(false, else_code, 
			method_name, "else-parameter must be junction");
		r.write_pass_lang(r.process(else_code));
	}
}

static void _untaint(Request& r, const String& method_name, Array *params) {
	const String& lang_name=r.process(*static_cast<Value *>(params->get(0))).as_string();
	String::Untaint_lang lang=static_cast<String::Untaint_lang>(
		untaint_lang_name2enum->get_int(lang_name));
	if(!lang)
		RTHROW(0, 0,
			&lang_name,
			"invalid untaint language");

	{
		Value *vbody=static_cast<Value *>(params->get(1));
		// forcing ^untaint[]{this param type}
		r.fail_if_junction_(false, *vbody, 
			method_name, "body must be junction");
		
		Temp_lang temp_lang(r, lang); // set temporarily specified ^untaint[language;
		r.write_pass_lang(r.process(*vbody)); // process marking tainted with that lang
	}
}

static void _taint(Request& r, const String& method_name, Array *params) {
	const String& lang_name=r.process(*static_cast<Value *>(params->get(0))).as_string();
	String::Untaint_lang lang=static_cast<String::Untaint_lang>(
		untaint_lang_name2enum->get_int(lang_name));
	if(!lang)
		RTHROW(0, 0,
			&lang_name,
			"invalid taint language");

	{
		Value *vbody=static_cast<Value *>(params->get(1));
		// forcing ^untaint[]{this param type}
		r.fail_if_junction_(false, *vbody, 
			method_name, "body must be junction");
		
		Temp_lang temp_lang(r, String::Untaint_lang::AS_IS); // set temporarily as-is language
		String result(r.process(*vbody).as_string()); // process marking tainted with that lang
		result.change_lang(lang); // switch result language to specified
		r.write_pass_lang(result);
	}
}

static void _process(Request& r, const String& method_name, Array *params) {
	// calculate pseudo file name of processed chars
	// would be something like "/some/file(4) process"
	char place[MAX_STRING];
#ifndef NO_STRING_ORIGIN
	const Origin& origin=method_name.origin();
	snprintf(place, MAX_STRING, "%s(%d) %s", 
		origin.file, 1+origin.line,
		method_name.cstr());
#else
	strncpy(place, MAX_STRING, method_name.cstr());
#endif	

	VStateless_class& self_class=*r.self->get_class();
	{
		// temporary zero @main so to maybe-replace it in processed code
		Temp_method temp_method(self_class, *main_method_name, 0);
		
		// evaluate source to process
		const String& source=
			r.process(*static_cast<Value *>(params->get(0))).as_string();

		// process source code, append processed methods to 'self' class
		// maybe-define new @main
		r.use_buf(source.cstr(), place, &self_class);
		
		// maybe-execute @main[]
		if(const Method *method=self_class.get_method(*main_method_name)) {
			// execute!	
			r.execute(*method->parser_code);
		}
	}
}
	
static void _rem(Request& r, const String& method_name, Array *params) {
	// forcing ^rem{this param type}
	r.fail_if_junction_(false, *static_cast<Value *>(params->get(0)), 
		method_name, "body must be junction");
}

static void _while(Request& r, const String& method_name, Array *params) {
	Value& vcondition=*static_cast<Value *>(params->get(0));
	// forcing ^while(this param type){}
	r.fail_if_junction_(false, vcondition, 
		method_name, "condition must be junction");
	
	Value& body=*static_cast<Value *>(params->get(1));
	// forcing ^while(){this param type}
	r.fail_if_junction_(false, body, 
		method_name, "body must be junction");

	// while...
	int endless_loop_count=0;
	while(true) {
		if(++endless_loop_count>=1973) // endless loop?
			RTHROW(0, 0,
				&method_name,
				"endless loop detected");

		bool condition=
			r.process(
				vcondition, 
				0/*no name*/,
				false/*don't intercept string*/).get_bool();
		if(!condition) // ...condition is true
			break;

		// write processed body
		r.write_pass_lang(r.process(body));
	}
}

static void _use(Request& r, const String& method_name, Array *params) {
	Value& vfile=*static_cast<Value *>(params->get(0));
	// forcing ^rem{this param type}
	r.fail_if_junction_(true, vfile, 
		method_name, "file name must not be junction");

	char *file=vfile.as_string().cstr();
	r.use_file(r.absolute(file));
}

static void _for(Request& r, const String& method_name, Array *params) {
	// ^for[i;from-number;to-number-inclusive]{code}[delim]

	Pool& pool=r.pool();
	const String& var_name=r.process(*static_cast<Value *>(params->get(0))).as_string();
	int from=(int)r.process(*static_cast<Value *>(params->get(1))).get_double();
	int to=(int)r.process(*static_cast<Value *>(params->get(2))).get_double();
	Value& body_code=*static_cast<Value *>(params->get(3));
	// forcing ^menu{this param type}
	r.fail_if_junction_(false, body_code, 
		method_name, "body must be junction");
	Value *delim_code=params->size()==3+1+1?static_cast<Value *>(params->get(3+1)):0;

	bool need_delim=false;
	VInt *vint=new(pool) VInt(pool, 0);
	int endless_loop_count=0;
	for(int i=from; i<=to; i++) {
		if(++endless_loop_count>=2001) // endless loop?
			RTHROW(0, 0,
				&method_name,
				"endless loop detected");
		vint->set_int(i);
		r.wcontext->put_element(var_name, vint);

		Value& processed_body=r.process(body_code);
		if(delim_code) { // delimiter set?
			const String *string=processed_body.get_string();
			if(need_delim && string && string->size()) // need delim & iteration produced string?
				r.write_pass_lang(r.process(*delim_code));
			need_delim=true;
		}
		r.write_pass_lang(processed_body);
	}
}

static void _eval(Request& r, const String& method_name, Array *params) {
	Value& expr=*static_cast<Value *>(params->get(0));
	r.fail_if_junction_(false, expr, 
		method_name, "need expression");
	// evaluate expresion
	Value *result=r.process(expr, 
		0/*no name*/,
		true/*don't intercept string*/).get_expr_result();
	if(params->size()==2) {
		Value& fmt=*static_cast<Value *>(params->get(1));
		// forcing ^format[this param type]
		r.fail_if_junction_(true, fmt, 
			method_name, "fmt must not be junction");

		Pool& pool=r.pool();
		String& string=*new(pool) String(pool);
		string.APPEND_CONST(format(pool, result->get_double(), fmt.as_string().cstr()));
		result=new(pool) VString(string);
	}
	r.write_no_lang(*result);
}


typedef double (*math_one_double_op_func_ptr)(double);
static double round(double op) { return floor(op+0.5); }
static double sign(double op) { return op > 0 ? 1 : ( op < 0 ? -1 : 0 ); }

static void double_one_op(
								Request& r, 
								const String& method_name, Array *params,
								math_one_double_op_func_ptr func) {
	Pool& pool=r.pool();
	Value& param=*static_cast<Value *>(params->get(0));

	// forcing ^round(this param type)
	r.fail_if_junction_(false, param, 
		method_name, "parameter must be expression");

	Value& result=*new(pool) VDouble(pool, (*func)(r.process(param).get_double()));
	r.write_no_lang(result);
}

static void _round(Request& r, const String& method_name, Array *params) {
	double_one_op(r, method_name, params,	&round);
}

static void _floor(Request& r, const String& method_name, Array *params) {
	double_one_op(r, method_name, params,	&floor);
}

static void _ceiling(Request& r, const String& method_name, Array *params) {
	double_one_op(r, method_name, params,	&ceil);
}

static void _abs(Request& r, const String& method_name, Array *params) {
	double_one_op(r, method_name, params,	&fabs);
}

static void _sign(Request& r, const String& method_name, Array *params) {
	double_one_op(r, method_name, params,	&sign);
}

// initialize

void initialize_root_class(Pool& pool, VStateless_class& vclass) {
	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	vclass.add_native_method("if", _if, 2, 3);

	// ^untaint[as-is|uri|sql|js|html|html-typo]{code}
	vclass.add_native_method("untaint", _untaint, 2, 2);

	// ^taint[as-is|uri|sql|js|html|html-typo]{code}
	vclass.add_native_method("taint", _taint, 2, 2);

	// ^process[code]
	vclass.add_native_method("process", _process, 1, 1);

	// ^rem{code}
	vclass.add_native_method("rem", _rem, 1, 1);

	// ^while(condition){code}
	vclass.add_native_method("while", _while, 2, 2);

	// ^use[file]
	vclass.add_native_method("use", _use, 1, 1);

	// ^for[i;from-number;to-number-inclusive]{code}[delim]
	vclass.add_native_method("for", _for, 3+1, 3+1+1);

	// ^eval(expr)
	// ^eval(expr)[format]
	vclass.add_native_method("eval", _eval, 1, 2);


	// math functions

	// ^round(expr)	
	vclass.add_native_method("round", _round, 1, 1);

	// ^floor(expr)	
	vclass.add_native_method("floor", _floor, 1, 1);

	// ^ceiling(expr)	
	vclass.add_native_method("ceiling", _ceiling, 1, 1);

	// ^abs(expr)	
	vclass.add_native_method("abs", _abs, 1, 1);

	// ^sign(expr)
	vclass.add_native_method("sign", _sign, 1, 1);
}
