/** @file
	Parser: parser @b operators.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: op.C,v 1.38 2001/08/09 14:27:49 parser Exp $"; 

#include "classes.h"
#include "pa_config_includes.h"
#include "pa_common.h"
#include "pa_request.h"
#include "pa_vint.h"
#include "pa_sql_connection.h"

// limits

#define MAX_LOOPS 10000

// defines

#define OP_CLASS_NAME "OP"

#define MAIN_SQL_NAME "SQL"
#define MAIN_SQL_DRIVERS_NAME "drivers"

// class

class MOP : public Methoded {
public:
	MOP(Pool& pool);
public: // Methoded
	bool used_directly() { return true; }
	void configure_user(Request& r);

private:
	String main_sql_name;
	String main_sql_drivers_name;
};

// methods

static void _if(Request& r, const String&, MethodParams *params) {
	Value& condition_code=params->get(0);

	bool condition=r.process(condition_code, 
		0/*no name*/,
		false/*don't intercept string*/).as_bool();
	if(condition)
		r.write_pass_lang(r.process(params->as_junction(1, "'then' parameter must be code")));
	else if(params->size()==3)
		r.write_pass_lang(r.process(params->as_junction(2, "'else' parameter must be code")));
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
		Value& vbody=params->as_junction(1, "body must be code");
		
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
		Value& vbody=params->as_no_junction(params->size()-1, "body must not be code");
		
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
	params->as_junction(0, "body must be code");
}

static void _while(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Value& vcondition=params->as_junction(0, "condition must be expression");
	Value& body=params->as_junction(1, "body must be code");

	// while...
	int endless_loop_count=0;
	while(true) {
		if(++endless_loop_count>=MAX_LOOPS) // endless loop?
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
	Value& vfile=params->as_no_junction(0, "file name must not be code");
	r.use_file(vfile.as_string());
}

static void _for(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	const String& var_name=r.process(params->get(0)).as_string();
	int from=r.process(params->get(1)).as_int();
	int to=r.process(params->get(2)).as_int();
	Value& body_code=params->as_junction(3, "body must be code");
	Value *delim_code=params->size()==3+1+1?&params->get(3+1):0;

	bool need_delim=false;
	VInt *vint=new(pool) VInt(pool, 0);
	int endless_loop_count=0;
	for(int i=from; i<=to; i++) {
		if(++endless_loop_count>=MAX_LOOPS) // endless loop?
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

static void _eval(Request& r, const String& method_name, MethodParams *params) {
	Value& expr=params->as_junction(0, "need expression");
	// evaluate expresion
	Value *result=r.process(expr, 
		0/*no name YET*/,
		true/*don't intercept string*/).as_expr_result();
	if(params->size()==2) {
		Value& fmt=params->as_no_junction(1, "fmt must not be code");

		Pool& pool=r.pool();
		String& string=*new(pool) String(pool);
		string.APPEND_CONST(format(pool, result->as_double(), fmt.as_string().cstr()));
		result=new(pool) VString(string);
	}
	result->set_name(method_name);
	r.write_no_lang(*result);
}


static void _connect(Request& r, const String&, MethodParams *params) {
	Pool& pool=r.pool();

	Value& url=params->as_no_junction(0, "url must not be code");
	Value& body_code=params->as_junction(1, "body must be code");

	Table *protocol2driver_and_client=
		static_cast<Table *>(r.classes_conf.get(r.OP.name()));

	// connect
	SQL_Connection& connection=SQL_driver_manager->get_connection(
		url.as_string(), protocol2driver_and_client);

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

struct Switch_data {
	Value *searching;
	Value *found;
	Value *_default;
};

static void _switch(Request& r, const String&, MethodParams *params) {
	void *backup=r.classes_conf.get(*switch_data_name);
	Switch_data data={&r.process(params->get(0))};	
	r.classes_conf.put(*switch_data_name, &data);

	r.process(params->as_junction(1, "switch cases must be code")); // and ignore result

	r.classes_conf.put(*switch_data_name, backup);

	if(Value *code=data.found ? data.found : data._default)
		r.write_pass_lang(r.process(*code));
}

static void _case(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();

	Switch_data *data=static_cast<Switch_data *>(r.classes_conf.get(*switch_data_name));
	if(!data)
		PTHROW(0, 0,
			&method_name,
			"without switch");

	int count=params->size();
	Value *code=&params->as_junction(--count, "case result must be code");
	for(int i=0; i<count; i++) {
		Value& value=r.process(params->get(i));

		if(value.as_string() == *case_default_value) {
			data->_default=code;
			break;
		}

		bool matches;
		if(data->searching->is_string())
			matches=data->searching->as_string() == value.as_string();
		else
			matches=data->searching->as_double() == value.as_double();

		if(matches) {
			data->found=code;
			break;
		}
	}
}

// constructor

MOP::MOP(Pool& apool) : Methoded(apool),
	main_sql_name(apool, MAIN_SQL_NAME),
	main_sql_drivers_name(apool, MAIN_SQL_DRIVERS_NAME)
{
	set_name(*NEW String(pool(), OP_CLASS_NAME));

	// ^if(condition){code-when-true}
	// ^if(condition){code-when-true}{code-when-false}
	add_native_method("if", Method::CT_ANY, _if, 2, 3);

	// ^untaint[as-is|uri|sql|js|html|html-typo]{code}
	add_native_method("untaint", Method::CT_ANY, _untaint, 2, 2);

	// ^taint[as-is|uri|sql|js|html|html-typo]{code}
	add_native_method("taint", Method::CT_ANY, _taint, 1, 2);

	// ^process[code]
	add_native_method("process", Method::CT_ANY, _process, 1, 1);

	// ^rem{code}
	add_native_method("rem", Method::CT_ANY, _rem, 1, 1000);

	// ^while(condition){code}
	add_native_method("while", Method::CT_ANY, _while, 2, 2);

	// ^use[file]
	add_native_method("use", Method::CT_ANY, _use, 1, 1);

	// ^for[i;from-number;to-number-inclusive]{code}[delim]
	add_native_method("for", Method::CT_ANY, _for, 3+1, 3+1+1);

	// ^eval(expr)
	// ^eval(expr)[format]
	add_native_method("eval", Method::CT_ANY, _eval, 1, 2);


	// ^connect[protocol://user:pass@host[:port]/database]{code with ^sql-s}
	add_native_method("connect", Method::CT_ANY, _connect, 2, 2);

	// switch

	// ^switch[value]{cases}
	add_native_method("switch", Method::CT_ANY, _switch, 2, 2);

	// ^case[value]{code}
	add_native_method("case", Method::CT_ANY, _case, 2, 1000);
}

// constructor & configurator

Methoded *MOP_create(Pool& pool) {
	return new(pool) MOP(pool);
}


void MOP::configure_user(Request& r) {
	Pool& pool=r.pool();

	// $MAIN:SQL.drivers
	if(Value *sql=r.main_class->get_element(main_sql_name))
		if(Value *element=sql->get_element(main_sql_drivers_name))
			if(Table *protocol2library=element->get_table())
				r.classes_conf.put(name(), protocol2library);
}
