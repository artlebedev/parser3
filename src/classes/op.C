/** @file
	Parser: @b ROOT parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: op.C,v 1.6 2001/04/15 13:12:17 paf Exp $
*/

#include "pa_config_includes.h"
#include <math.h>

#include "pa_common.h"
#include "pa_request.h"
#include "_op.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"

static void _if(Request& r, const String&, MethodParams *params) {
	Value& condition_code=params->get(0);

	bool condition=r.process(condition_code, 
		0/*no name*/,
		false/*don't intercept string*/).as_bool();
	if(condition)
		r.write_pass_lang(r.process(params->get(1)));
	else if(params->size()==3)
		r.write_pass_lang(r.process(params->get(2)));
}

static void _untaint(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	const String& lang_name=r.process(params->get(0)).as_string();
	String::Untaint_lang lang=static_cast<String::Untaint_lang>(
		untaint_lang_name2enum->get_int(lang_name));
	if(!lang)
		PTHROW(0, 0,
			&lang_name,
			"invalid untaint language");

	{
		Value& vbody=params->get_junction(1, "body must be code");
		
		Temp_lang temp_lang(r, lang); // set temporarily specified ^untaint[language;
		r.write_pass_lang(r.process(vbody)); // process marking tainted with that lang
	}
}

static void _taint(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	String::Untaint_lang lang;
	if(params->size()==1)
		lang=String::UL_TAINTED; // mark as simply 'tainted'. useful in table:set
	else {
		const String& lang_name=
			r.process(params->get(0)).as_string();
		lang=static_cast<String::Untaint_lang>(
			untaint_lang_name2enum->get_int(lang_name));
		if(!lang)
			PTHROW(0, 0,
				&lang_name,
				"invalid taint language");
	}

	{
		Value& vbody=params->get_no_junction(params->size()-1, "body must not be code");
		
		String result(r.pool());
		result.append(
			vbody.as_string(),  // process marking tainted with that lang
			lang, true);  // force result language to specified
		r.write_pass_lang(result);
	}
}

static void _process(Request& r, const String& method_name, MethodParams *params) {
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
		Temp_method temp_method_main(self_class, *main_method_name, 0);
		// temporary zero @auto so it wouldn't be auto-called in Request::use_buf
		Temp_method temp_method_auto(self_class, *auto_method_name, 0);
		
		// evaluate source to process
		const String& source=
			r.process(params->get(0)).as_string();

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
	
static void _rem(Request& r, const String&, MethodParams *params) {
	params->get_junction(0, "body must be code");
}

static void _while(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& vcondition=params->get_junction(0, "condition must be expression");
	Value& body=params->get_junction(1, "body must be code");

	// while...
	int endless_loop_count=0;
	while(true) {
		if(++endless_loop_count>=1973) // endless loop?
			PTHROW(0, 0,
				&method_name,
				"endless loop detected");

		bool condition=
			r.process(
				vcondition, 
				0/*no name*/,
				false/*don't intercept string*/).as_bool();
		if(!condition) // ...condition is true
			break;

		// write processed body
		r.write_pass_lang(r.process(body));
	}
}

static void _use(Request& r, const String& method_name, MethodParams *params) {
	Value& vfile=params->get_no_junction(0, "file name must not be code");
	r.use_file(r.absolute(vfile.as_string()));
}

/// ^for[i;from-number;to-number-inclusive]{code}[delim]
static void _for(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& var_name=r.process(params->get(0)).as_string();
	int from=(int)r.process(params->get(1)).as_double();
	int to=(int)r.process(params->get(2)).as_double();
	Value& body_code=params->get_junction(3, "body must be code");
	Value *delim_code=params->size()==3+1+1?&params->get(3+1):0;

	bool need_delim=false;
	VInt *vint=new(pool) VInt(pool, 0);
	int endless_loop_count=0;
	for(int i=from; i<=to; i++) {
		if(++endless_loop_count>=2001) // endless loop?
			PTHROW(0, 0,
				&method_name,
				"endless loop detected");
		vint->set_int(i);
		r.root->put_element(var_name, vint);

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

static void _eval(Request& r, const String&, MethodParams *params) {
	Value& expr=params->get_junction(0, "need expression");
	// evaluate expresion
	Value *result=r.process(expr, 
		0/*no name*/,
		true/*don't intercept string*/).as_expr_result();
	if(params->size()==2) {
		Value& fmt=params->get_no_junction(1, "fmt must not be code");

		Pool& pool=r.pool();
		String& string=*new(pool) String(pool);
		string.APPEND_CONST(format(pool, result->as_double(), fmt.as_string().cstr()));
		result=new(pool) VString(string);
	}
	r.write_no_lang(*result);
}


typedef double (*math_one_double_op_func_ptr)(double);
static double round(double op) { return floor(op+0.5); }
static double sign(double op) { return op > 0 ? 1 : ( op < 0 ? -1 : 0 ); }

static void double_one_op(
								Request& r, 
								const String& method_name, MethodParams *params,
								math_one_double_op_func_ptr func) {
	Pool& pool=r.pool();
	Value& param=params->get_junction(0, "parameter must be expression");

	Value& result=*new(pool) VDouble(pool, (*func)(r.process(param).as_double()));
	r.write_no_lang(result);
}

static void _round(Request& r, const String& method_name, MethodParams *params) {
	double_one_op(r, method_name, params,	&round);
}

static void _floor(Request& r, const String& method_name, MethodParams *params) {
	double_one_op(r, method_name, params,	&floor);
}

static void _ceiling(Request& r, const String& method_name, MethodParams *params) {
	double_one_op(r, method_name, params,	&ceil);
}

static void _abs(Request& r, const String& method_name, MethodParams *params) {
	double_one_op(r, method_name, params,	&fabs);
}

static void _sign(Request& r, const String& method_name, MethodParams *params) {
	double_one_op(r, method_name, params,	&sign);
}

/// ^connect[protocol://user:pass@host[:port]/database]{code with ^sql-s}
/**
	@test make params not Array but something with useful method for extracting,
	with typecast and junction/not test
*/
static void _connect(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	Value& url=params->get_no_junction(0, "url must not be code");
	Value& body_code=params->get_junction(1, "body must be code");

	// connect
	SQL_Connection& connection=SQL_driver_manager->get_connection(
		url.as_string(), r.protocol2library);

	Exception rethrow_me;
	// remember/set current connection
	SQL_Connection *saved_connection=r.connection;
	r.connection=&connection;
	// execute body
	bool body_failed=false;  
	PTRY
		r.write_assign_lang(r.process(body_code));
	PCATCH(e) { // connect/process problem
		rethrow_me=e;  body_failed=true; 
	}
	PEND_CATCH

	bool finalizer_failed=false;
	PTRY
		// FINALLY
		if(body_failed)
			connection.rollback();
		else
			connection.commit();
	PCATCH(e) { // commit/rollback problem
		rethrow_me=e;  finalizer_failed=true; 
	}
	PEND_CATCH

	// close connection [cache it]
	connection.close();
	// recall current connection from remembered
	r.connection=saved_connection;

	if(body_failed || finalizer_failed) // were there an exception for us to rethrow?
		PTHROW(rethrow_me.type(), rethrow_me.code(),
			rethrow_me.problem_source(),
			rethrow_me.comment());
}

// initialize

void initialize_op_class(Pool& pool, VStateless_class& vclass) {
	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	vclass.add_native_method("if", Method::CT_ANY, _if, 2, 3);

	// ^untaint[as-is|uri|sql|js|html|html-typo]{code}
	vclass.add_native_method("untaint", Method::CT_ANY, _untaint, 2, 2);

	// ^taint[as-is|uri|sql|js|html|html-typo]{code}
	vclass.add_native_method("taint", Method::CT_ANY, _taint, 1, 2);

	// ^process[code]
	vclass.add_native_method("process", Method::CT_ANY, _process, 1, 1);

	// ^rem{code}
	vclass.add_native_method("rem", Method::CT_ANY, _rem, 1, 1);

	// ^while(condition){code}
	vclass.add_native_method("while", Method::CT_ANY, _while, 2, 2);

	// ^use[file]
	vclass.add_native_method("use", Method::CT_ANY, _use, 1, 1);

	// ^for[i;from-number;to-number-inclusive]{code}[delim]
	vclass.add_native_method("for", Method::CT_ANY, _for, 3+1, 3+1+1);

	// ^eval(expr)
	// ^eval(expr)[format]
	vclass.add_native_method("eval", Method::CT_ANY, _eval, 1, 2);


	// math functions

	// ^round(expr)	
	vclass.add_native_method("round", Method::CT_ANY, _round, 1, 1);

	// ^floor(expr)	
	vclass.add_native_method("floor", Method::CT_ANY, _floor, 1, 1);

	// ^ceiling(expr)	
	vclass.add_native_method("ceiling", Method::CT_ANY, _ceiling, 1, 1);

	// ^abs(expr)	
	vclass.add_native_method("abs", Method::CT_ANY, _abs, 1, 1);

	// ^sign(expr)
	vclass.add_native_method("sign", Method::CT_ANY, _sign, 1, 1);

	
	// connect

	// ^connect[protocol://user:pass@host[:port]/database]{code with ^sql-s}
	vclass.add_native_method("connect", Method::CT_ANY, _connect, 2, 2);

}
