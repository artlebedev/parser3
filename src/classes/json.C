/** @file
	Parser: @b json parser class.

	Copyright (c) 2010 ArtLebedev Group (http://www.artlebedev.com)
*/

static const char * const IDENT_RESPONSE_C="$Date: 2010/09/09 09:52:43 $";

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vbool.h"

#include "pa_charset.h"
#include "pa_charsets.h"
#include "JSON_parser.h"

// class

class MJson: public Methoded {
public:
	MJson();
public: // Methoded
	bool used_directly() { return true; }
};

// global variable

DECLARE_CLASS_VAR(json, new MJson, 0);

// methods
struct Json {
	Stack<VHash*> stack;
	Stack<String*> key_stack;

	String* key;
	Value* result;

	Junction* hook;
	Request* request;

	Charset *charset;
	bool handle_double;
	enum Distinct { D_EXCEPTION, D_FIRST, D_LAST, D_ALL } distinct;

	Json(Charset* acharset): stack(), key_stack(), key(NULL), result(NULL), hook(NULL), request(NULL), charset(acharset), handle_double(true), distinct(D_EXCEPTION){}

	bool set_distinct(const String &value){
		if (value == "first") distinct = D_FIRST;
		else if (value == "last") distinct = D_LAST;
		else if (value == "all") distinct = D_ALL;
		else return false;
		return true;
	}
};

static void set_json_value(Json *json, Value *value){
	VHash *top = json->stack.top_value();
	if(json->key == NULL){
		top->hash().put(String(format(top->get_hash()->count(), 0)), value);
	} else {
		switch (json->distinct){
			case Json::D_EXCEPTION:
				if (top->hash().put_dont_replace(*json->key, value))
					throw Exception(PARSER_RUNTIME, json->key, "duplicate key");
				break;
			case Json::D_FIRST:
				top->hash().put_dont_replace(*json->key, value);
				break;
			case Json::D_LAST:
				top->hash().put(*json->key, value);
				break;
			case Json::D_ALL:
				if (top->hash().put_dont_replace(*json->key, value)){
					for(int i=2;;i++){
						String key;
						key << *json->key << "_" << format(i, 0);
						if (!top->hash().put_dont_replace(key, value)) break;
					}
				}
				break;
		}
		json->key=NULL;
	}
}

String* json_string(Json *json, const JSON_value* value){
	String::C result = json->charset !=NULL ? 
			Charset::transcode(String::C(value->vu.str.value, value->vu.str.length), UTF8_charset, *json->charset) :
			String::C(pa_strdup(value->vu.str.value, value->vu.str.length), value->vu.str.length);
	return new String(result.str, String::L_TAINTED, result.length);
}

static Value *json_hook(Request &r, Junction *hook, String* key, Value* value){
	VMethodFrame frame(*hook->method, r.method_frame, hook->self);
	Value *params[]={new VString(*key), value};

	frame.store_params(params, 2);
	r.execute_method(frame);

	return &frame.result().as_value();
}

static int json_callback(Json *json, int type, const JSON_value* value)
{
	switch(type) {
		case JSON_T_OBJECT_BEGIN:{
			VHash *v = new VHash();
			if (json->hook){
				json->key_stack.push(json->key);
			} else {
				if (json->stack.count()) set_json_value(json, v);
			}
			json->stack.push(v);
			break;
		}
		case JSON_T_OBJECT_END:{
			if (json->hook){
				String* key = json->key_stack.pop();
				json->result = json_hook(*json->request, json->hook, key, json->stack.pop());
				
				if (json->stack.count()){
					json->key = key;
					set_json_value(json, json->result);
				}
			} else {
				json->result = json->stack.pop();
			}
			break;
		}
		case JSON_T_ARRAY_BEGIN:{
			VHash *v = new VHash();
			set_json_value(json, v);
			json->stack.push(v);
			break;
		}
		case JSON_T_ARRAY_END:
			json->stack.pop();
			break;
		case JSON_T_KEY:
			json->key = json_string(json, value);
			break;  
		case JSON_T_INTEGER:
			set_json_value(json, new VInt((int)value->vu.integer_value));
			break;
		case JSON_T_FLOAT:
			if (json->handle_double){
				set_json_value(json, new VDouble( json_string(json, value)->as_double() ));
				break;
			} // else is JSON_T_STRING
		case JSON_T_STRING:
			set_json_value(json, new VString(*json_string(json, value)));
			break;
		case JSON_T_NULL:
			set_json_value(json, new VVoid());
			break;
		case JSON_T_TRUE:
			set_json_value(json, &VBool::get(true));
			break;
		case JSON_T_FALSE:
			set_json_value(json, &VBool::get(false));
			break; 
	}
	return 1;
}

static const char* json_error_message(int error_code){
	static const char* error_messages[] = {
		NULL,
		"invalid char",
		"invalid keyword",
		"invalid escape sequence",
		"invalid unicode sequence",
		"invalid number",
		"nesting depth reached",
		"unbalanced collection",
		"expected key",
		"expected colon",
		"out of memory"
	};
	return error_messages[error_code];
}

static void _parse(Request& r, MethodParams& params) {
	const String& json_string=params.as_string(0, "json must be string");

	Json json(r.charsets.source().isUTF8() ? NULL : &(r.charsets.source()));

	JSON_config config;
	init_JSON_config(&config);

	config.depth                  = 19;
	config.callback               = (JSON_parser_callback)&json_callback;
	config.allow_comments         = 1;
	config.handle_floats_manually = 1;
	config.callback_ctx           = &json;

	if(params.count() == 2)
		if(HashStringValue* options=params.as_hash(1)) {
			int valid_options=0;
			if(Value* value=options->get("depth")) {
				config.depth=r.process_to_value(*value).as_int();
				valid_options++;
			}
			if(Value* value=options->get("double")) {
				json.handle_double=r.process_to_value(*value).as_bool();
				valid_options++;
			}
			if(Value* value=options->get("distinct")) {
				const String& sdistinct=value->as_string();
				if (!json.set_distinct(sdistinct))
					throw Exception(PARSER_RUNTIME, &sdistinct, "must be 'first', 'last' or 'all'");
				valid_options++;
			}
			if(Value* value=options->get("object")) {
				json.hook=value->get_junction();
				json.request=&r;
				if (!json.hook || !json.hook->method || !json.hook->method->params_names || !(json.hook->method->params_names->count() == 2))
					throw Exception(PARSER_RUNTIME, 0, "$.object must be parser method with 2 parameters");
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	const String::Body json_body = json_string.cstr_to_string_body_untaint(String::L_JSON, 0, &(r.charsets));
	const char *json_cstr = json.charset != NULL ? Charset::transcode(json_body, *json.charset, UTF8_charset).cstr() : json_body.cstr();

	struct JSON_parser_struct* jc = new_JSON_parser(&config);

	for (const char *c=json_cstr; *c; c++){
		if (!JSON_parser_char(jc, *((const unsigned char *)c))) {
			throw Exception("json.parse", 0, "%s at byte %d", json_error_message(JSON_parser_get_last_error(jc)), c-json_cstr);
		}
	}

	if (!JSON_parser_done(jc)) {
		throw Exception("json.parse", 0, "%s at the end", json_error_message(JSON_parser_get_last_error(jc)));
	}
	
	delete_JSON_parser(jc);

	if (json.result) r.write_no_lang(*json.result);
}

// constructor

MJson::MJson(): Methoded("json") {
	add_native_method("parse", Method::CT_STATIC, _parse, 1, 2);
}
