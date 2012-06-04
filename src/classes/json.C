/** @file
	Parser: @b json parser class.

	Copyright (c) 2000-2012 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vbool.h"

#include "pa_charset.h"
#include "pa_charsets.h"
#include "JSON_parser.h"

#ifdef XML
#include "pa_vxdoc.h"
#endif

volatile const char * IDENT_JSON_C="$Id: json.C,v 1.23 2012/06/04 13:46:18 moko Exp $";

// class

class MJson: public Methoded {
public:
	MJson();
};

// global variable

DECLARE_CLASS_VAR(json, new MJson, 0);

// methods
struct Json {
	Stack<VHash*> stack;
	Stack<String*> key_stack;

	String* key;
	Value* result;

	Junction* hook_object;
	Junction* hook_array;
	Request* request;

	Charset *charset;
	String::Language taint;

	bool handle_double;
	enum Distinct { D_EXCEPTION, D_FIRST, D_LAST, D_ALL } distinct;

	Json(Charset* acharset): stack(), key_stack(), key(NULL), result(NULL), hook_object(NULL), hook_array(NULL), 
		request(NULL), charset(acharset), taint(String::L_TAINTED), handle_double(true), distinct(D_EXCEPTION){}

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
	return new String(result.str, json->taint, result.length);
}

static Value *json_hook(Request &r, Junction *hook, String* key, Value* value){
	VMethodFrame frame(*hook->method, r.method_frame, hook->self);
	Value *params[]={new VString(key ? *key : String::Empty), value};

	frame.store_params(params, 2);
	r.execute_method(frame);

	return &frame.result().as_value();
}

static int json_callback(Json *json, int type, const JSON_value* value)
{
	switch(type) {
		case JSON_T_OBJECT_BEGIN:{
			VHash *v = new VHash();
			if (json->hook_object){
				json->key_stack.push(json->key);
				json->key=NULL;
			} else {
				if (json->stack.count()) set_json_value(json, v);
			}
			json->stack.push(v);
			break;
		}
		case JSON_T_OBJECT_END:{
			if (json->hook_object){
				String* key = json->key_stack.pop();
				json->result = json_hook(*json->request, json->hook_object, key, json->stack.pop());
				
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
			if (json->hook_array){
				json->key_stack.push(json->key);
				json->key=NULL;
			} else {
				if (json->stack.count()) set_json_value(json, v);
			}
			json->stack.push(v);
			break;
		}
		case JSON_T_ARRAY_END:
			// libjson supports array at top level, we too
			if (json->hook_array){
				String* key = json->key_stack.pop();
				json->result = json_hook(*json->request, json->hook_array, key, json->stack.pop());
				
				if (json->stack.count()){
					json->key = key;
					set_json_value(json, json->result);
				}
			} else {
				json->result = json->stack.pop();
			}
			break;
		case JSON_T_KEY:
			json->key = json_string(json, value);
			break;
		case JSON_T_INTEGER:
			set_json_value(json, new VDouble((double)value->vu.integer_value));
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
			set_json_value(json, VVoid::get());
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

extern String::Language get_untaint_lang(const String& lang_name);

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
			if(Value* value=options->get("taint")) {
				json.taint=get_untaint_lang(value->as_string());
				valid_options++;
			}
			if(Value* value=options->get("object")) {
				json.hook_object=value->get_junction();
				json.request=&r;
				if (!json.hook_object || !json.hook_object->method || !json.hook_object->method->params_names || !(json.hook_object->method->params_names->count() == 2))
					throw Exception(PARSER_RUNTIME, 0, "$.object must be parser method with 2 parameters");
				valid_options++;
			}
			if(Value* value=options->get("array")) {
				json.hook_array=value->get_junction();
				json.request=&r;
				if (!json.hook_array || !json.hook_array->method || !json.hook_array->method->params_names || !(json.hook_array->method->params_names->count() == 2))
					throw Exception(PARSER_RUNTIME, 0, "$.array must be parser method with 2 parameters");
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

char *get_indent(uint level){
	static char* cache[ANTI_ENDLESS_JSON_STRING_RECOURSION]={};
	if (!cache[level]){
		char *result = 	static_cast<char*>(pa_gc_malloc_atomic(level+1));
		memset(result, '\t', level);
		result[level]='\0';
		return cache[level]=result;
	}
	return cache[level];
}

const String& value_json_string(String::Body key, Value& v, Json_options& options);

const String* Json_options::hash_json_string(HashStringValue &hash) {
	if(!hash.count())
		return new String("{}", String::L_AS_IS);

	uint level = r->json_string_recoursion_go_down();

	String& result = *new String("{\n", String::L_AS_IS);

	if (indent){

		String *delim=NULL;
		indent=get_indent(level);
		for(HashStringValue::Iterator i(hash); i; i.next() ){
			if (delim){
				result << *delim;
			} else {
				result << indent << "\"";
				delim = new String(",\n", String::L_AS_IS); *delim << indent << "\"";
			}
			result << String(i.key(), String::L_JSON) << "\":" << value_json_string(i.key(), *i.value(), *this);
		}
		result << "\n" << (indent=get_indent(level-1)) << "}";

	} else {

		bool need_delim=false;
		for(HashStringValue::Iterator i(hash); i; i.next() ){
			result << (need_delim ? ",\n\"" : "\"");
			result << String(i.key(), String::L_JSON) << "\":" << value_json_string(i.key(), *i.value(), *this);
			need_delim=true;
		}
		result << "\n}";

	}

	r->json_string_recoursion_go_up();
	return &result;
}

static bool based_on(HashStringValue::key_type key, HashStringValue::value_type /*value*/, Value* v) {
	return v->is(key.cstr());
}
					
const String& value_json_string(String::Body key, Value& v, Json_options& options) {
	if(options.methods) {
		Value* method=options.methods->get(v.type());
		if(!method){
			method=options.methods->first_that<Value*>(based_on, &v);
			options.methods->put(key, method ? method : VVoid::get());
		}
		if(method && !method->is_void()) {
			Junction* junction=method->get_junction();
			VMethodFrame frame(*junction->method, options.r->method_frame, junction->self);

			Value *params[]={new VString(*new String(key, String::L_JSON)), &v, options.params ? options.params : VVoid::get()};
			frame.store_params(params, 3);

			options.r->execute_method(frame);

			return frame.result().as_string();
		}
	}

	options.key=key;
	return *v.get_json_string(options);
}

static void _string(Request& r, MethodParams& params) {
	Json_options json(&r);

	if(params.count() == 2)
		if(HashStringValue* options=params.as_hash(1)) {
			json.params=params.get(1);
			HashStringValue* methods=new HashStringValue();
			int valid_options=0;
			HashStringValue* vvalue;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();
				if(key == "skip-unknown"){
					json.skip_unknown=r.process_to_value(*value).as_bool();
					valid_options++;
				} else if(key == "date" && value->is_string()){
					const String& svalue=value->as_string();
					if(!json.set_date_format(svalue))
						throw Exception(PARSER_RUNTIME, &svalue, "must be 'sql-string', 'gmt-string' or 'unix-timestamp'");
					valid_options++;
				} else if(key == "indent"){
					json.indent=r.process_to_value(*value).as_bool() ? "":NULL;
					valid_options++;
				} else if(key == "table" && value->is_string()){
					const String& svalue=value->as_string();
					if(!json.set_table_format(svalue))
						throw Exception(PARSER_RUNTIME, &svalue, "must be 'array', 'object' or 'compact'");
					valid_options++;
				} else if(key == "file" && value->is_string()){
					const String& svalue=value->as_string();
					if(!json.set_file_format(svalue))
						throw Exception(PARSER_RUNTIME, &svalue, "must be 'base64', 'text' or 'stat'");
					valid_options++;
#ifdef XML
				} else if(key == "xdoc" && (vvalue = value->get_hash())){
					json.xdoc_options=new XDocOutputOptions(r, vvalue);
					valid_options++;
#endif
				} else if(Junction* junction=value->get_junction()){
					if(!junction->method || !junction->method->params_names || junction->method->params_names->count() != 3)
						throw Exception(PARSER_RUNTIME, 0, "$.%s must be parser method with 3 parameters", key.cstr());
					methods->put(key, value);
					valid_options++;
				}
			}

			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);

			// special handling for $._default 
			if(VHash* vhash=static_cast<VHash*>(params[1].as(VHASH_TYPE)))
				if(Value* value=vhash->get_default()) {
					Junction* junction=value->get_junction();
					if(!junction || !junction->method || !junction->method->params_names || junction->method->params_names->count() != 3)
						throw Exception(PARSER_RUNTIME, 0, "$.%s must be parser method with 3 parameters", HASH_DEFAULT_ELEMENT_NAME);
					json.default_method=value;
				}

			if(methods->count())
				json.methods=methods;
		}

	const String& result_string=value_json_string(String::Body(), params[0], json);
	String::Body result_body=result_string.cstr_to_string_body_untaint(String::L_JSON, 0, &r.charsets);
	r.write_pass_lang(*new String(result_body, String::L_AS_IS));
 }

// constructor

MJson::MJson(): Methoded("json") {
	add_native_method("parse", Method::CT_STATIC, _parse, 1, 2);

	add_native_method("string", Method::CT_ANY, _string, 1, 2);
}
