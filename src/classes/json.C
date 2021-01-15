/** @file
	Parser: @b json parser class.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "classes.h"
#include "pa_vmethod_frame.h"

#include "pa_request.h"
#include "pa_vbool.h"

#include "pa_charset.h"
#include "pa_charsets.h"
#include "pa_json.h"

#ifdef XML
#include "pa_vxdoc.h"
#endif

volatile const char * IDENT_JSON_C="$Id: json.C,v 1.56 2021/01/15 16:28:36 moko Exp $";

// class

class MJson: public Methoded {
public:
	MJson();
};

// global variable

DECLARE_CLASS_VAR(json, new MJson);

// methods
struct Json : public PA_Allocated {
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
	bool handle_int;
	enum Distinct { D_EXCEPTION, D_FIRST, D_LAST, D_ALL } distinct;

	Json(Charset* acharset): stack(), key_stack(), key(NULL), result(NULL), hook_object(NULL), hook_array(NULL), 
		request(NULL), charset(acharset), taint(String::L_TAINTED), handle_double(true), handle_int(true), 
		distinct(D_EXCEPTION){}

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
		top->hash().put(format(top->get_hash()->count(), 0), value);
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

String* json_string(Json *json, const char *value, uint32_t length){
	String::C result = json->charset !=NULL ? 
		Charset::transcode(String::C(value, length), pa_UTF8_charset, *json->charset) :
		String::C(pa_strdup(value, length), length);
	return new String(result, json->taint);
}

static Value *json_hook(Request &r, Junction *hook, String* key, Value* value){
	Value *params[]={new VString(key ? *key : String::Empty), value};
	METHOD_FRAME_ACTION(*hook->method, r.method_frame, hook->self, {
		frame.store_params(params, 2);
		r.call(frame);
		return &frame.result();
	});
}

static int json_callback(Json *json, int type, const char *value, uint32_t length)
{
	switch(type) {
		case JSON_OBJECT_BEGIN:{
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
		case JSON_OBJECT_END:{
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
		case JSON_ARRAY_BEGIN:{
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
		case JSON_ARRAY_END:
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
		case JSON_KEY:
			json->key = json_string(json, value, length);
			break;
		case JSON_INT:
			if (json->handle_int){
				set_json_value(json, new VDouble( json_string(json, value, length)->as_double() ));
			} else {
				// JSON_STRING
				set_json_value(json, new VString(*json_string(json, value, length)));
			}
			break;
		case JSON_FLOAT:
			if (json->handle_double){
				set_json_value(json, new VDouble( json_string(json, value, length)->as_double() ));
				break;
			} // else is JSON_STRING
		case JSON_STRING:
			set_json_value(json, new VString(*json_string(json, value, length)));
			break;
		case JSON_NULL:
			set_json_value(json, VVoid::get());
			break;
		case JSON_TRUE:
			set_json_value(json, &VBool::get(true));
			break;
		case JSON_FALSE:
			set_json_value(json, &VBool::get(false));
			break;
	}
	return 0;
}

static const char* json_error_message(int error_code){
	static const char* error_messages[] = {
		NULL,
		"out of memory",
		"bad character",
		"stack empty",
		"pop unexpected mode",
		"nesting limit",
		"data limit",
		"comment not allowed by config",
		"unexpected character",
		"missing unicode low surrogate",
		"unexpected unicode low surrogate",
		"error comma out of structure",
		"error in a callback"
	};
	return error_messages[error_code];
}

extern String::Language get_untaint_lang(const String& lang_name);

#define SOURCE_MAX_LEN 60

void json_exception_with_source(Request& r, const char* msg, const char* json, int offset){
	int i;

	int line=0;
	int start=0;
	int end=strlen(json);

	if(offset>end)
		offset=end;

	for(i = 0; i < offset; i++){
		if(json[i]=='\n'){
			line++;
		}
	}

	if(offset > SOURCE_MAX_LEN/2)
		start = offset - SOURCE_MAX_LEN/2;

	for(i = offset-1; i>=start; i--){
		if(json[i]=='\n'){
			start=i+1;
			break;
		}
	}

	if(start+SOURCE_MAX_LEN < end)
		end=start+SOURCE_MAX_LEN;

	for(i = offset+1; i<end; i++){
		if(json[i]=='\n'){
			end=i;
			break;
		}
	}

	char *source = pa_strdup(json+start, end-start);
	int source_offset = offset-start;

	if(source[source_offset]=='\n')
		source[source_offset]=' ';

	for(i = 0; i < source_offset; i++){
		if(source[i]=='\t'){
			source[i]=' ';
		}
	}

	if(r.charsets.source().isUTF8()){
		source=(char *)fixUTF8(source);
		if(source_offset>0){
			String s_source(pa_strdup(source,source_offset));
			source_offset=s_source.length(r.charsets.source());
		}
	}

	throw Exception("json.parse", 0, "%s at line %d\n%s\n%*s", msg, line+1, source, source_offset+1, "^");
}

static void _parse(Request& r, MethodParams& params) {
	const String& json_string=params.as_string(0, "json must be string");

	Json json(r.charsets.source().isUTF8() ? NULL : &(r.charsets.source()));

	json_config config = {
		0,		// buffer_initial_size
		128,		// max_nesting
		0,		// max_data
		1,		// allow_c_comments
		1,		// allow_yaml_comments
		pa_malloc,
		pa_realloc,
		pa_free
	};

	if(params.count() == 2)
		if(HashStringValue* options=params.as_hash(1)) {
			int valid_options=0;
			if(Value* value=options->get("depth")) {
				config.max_nesting=r.process(*value).as_int();
				valid_options++;
			}
			if(Value* value=options->get("double")) {
				json.handle_double=r.process(*value).as_bool();
				valid_options++;
			}
			if(Value* value=options->get("int")) {
				json.handle_int=r.process(*value).as_bool();
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
				if (!json.hook_object || !json.hook_object->method || !json.hook_object->method->params_names || !(json.hook_object->method->params_count == 2))
					throw Exception(PARSER_RUNTIME, 0, "$.object must be parser method with 2 parameters");
				valid_options++;
			}
			if(Value* value=options->get("array")) {
				json.hook_array=value->get_junction();
				json.request=&r;
				if (!json.hook_array || !json.hook_array->method || !json.hook_array->method->params_names || !(json.hook_array->method->params_count == 2))
					throw Exception(PARSER_RUNTIME, 0, "$.array must be parser method with 2 parameters");
				valid_options++;
			}
			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
		}

	const String::Body json_body = json_string.cstr_to_string_body_untaint(String::L_JSON, r.connection(false), &r.charsets);
	const char *json_cstr = json.charset != NULL ? Charset::transcode(json_body, *json.charset, pa_UTF8_charset).cstr() : json_body.cstr();

	json_parser parser;
	if(int result = json_parser_init(&parser, &config, (json_parser_callback)&json_callback, &json))
		throw Exception("json.parse", 0, "%s", json_error_message(result));

	if(!*json_cstr)
		throw Exception("json.parse", 0, "empty string is not valid json");

	const char *first_quote=strchr(json_cstr,'"');
	if(first_quote && first_quote>json_cstr && *(--first_quote) == '\\')
		json_exception_with_source(r, "illegal quote escape, json may be tainted", json_cstr, first_quote-json_cstr);

	uint32_t processed;
	if(int result = json_parser_string(&parser, json_cstr, strlen(json_cstr), &processed))
		json_exception_with_source(r, json_error_message(result), json_cstr, processed);

	if (!json_parser_is_done(&parser))
		json_exception_with_source(r, "unexpected end of json data", json_cstr, processed);

	json_parser_free(&parser);

	if (json.result) r.write(*json.result);
}

const uint ANTI_ENDLESS_JSON_STRING_RECOURSION=128;

char *get_indent(uint level){
	static char* cache[ANTI_ENDLESS_JSON_STRING_RECOURSION]={};
	if (!cache[level]){
		char *result = static_cast<char*>(pa_malloc_atomic(level+1));
		memset(result, '\t', level);
		result[level]='\0';
		return cache[level]=result;
	}
	return cache[level];
}

String *get_delim(uint level){
	static String* cache[ANTI_ENDLESS_JSON_STRING_RECOURSION]={};

	if (!cache[level]){
		char *result = static_cast<char*>(pa_malloc_atomic(level+2+1+1));
		result[0]=',';
		result[1]='\n';
		memset(result+2, '\t', level);
		result[level+2]='"';
		result[level+3]='\0';
		return cache[level] = new String(result, String::L_AS_IS);
	}
	return cache[level];
}

class Json_string_recoursion {
	Json_options& foptions;
public:
	Json_string_recoursion(Json_options& aoptions) : foptions(aoptions) {
		if(++foptions.json_string_recoursion==ANTI_ENDLESS_JSON_STRING_RECOURSION)
			throw Exception(PARSER_RUNTIME, 0, "call canceled - endless json recursion detected");
	}
	~Json_string_recoursion() {
		if(foptions.json_string_recoursion)
			foptions.json_string_recoursion--;
	}
};

const String& value_json_string(String::Body key, Value& v, Json_options& options);

const String* Json_options::hash_json_string(HashStringValue *hash) {
	if(!hash || !hash->count())
		return new String("{}", String::L_AS_IS);

	Json_string_recoursion go_down(*this);

	String& result = *new String("{\n", String::L_AS_IS);

	if (indent){

		String *delim=NULL;
		indent=get_indent(json_string_recoursion);
		for(HashStringValue::Iterator i(*hash); i; i.next() ){
			if (delim){
				result << *delim;
			} else {
				result << indent << "\"";
				delim = get_delim(json_string_recoursion);
			}
			result << String(i.key(), String::L_JSON) << "\":" << value_json_string(i.key(), *i.value(), *this);
		}
		result << "\n" << (indent=get_indent(json_string_recoursion-1)) << "}";

	} else {

		bool need_delim=false;
		for(HashStringValue::Iterator i(*hash); i; i.next() ){
			result << (need_delim ? ",\n\"" : "\"");
			result << String(i.key(), String::L_JSON) << "\":" << value_json_string(i.key(), *i.value(), *this);
			need_delim=true;
		}
		result << "\n}";

	}

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
			options.methods->put(v.type(), method ? method : VVoid::get());
		}
		if(method && !method->is_void()) {
			Junction* junction=method->get_junction();
			HashStringValue* params_hash=options.params && options.indent ? options.params->get_hash() : NULL;
			Temp_hash_value<HashStringValue, Value*> indent(params_hash, "indent", new VString(*new String(options.indent, String::L_AS_IS)));

			Value *params[]={new VString(*new String(key, String::L_JSON)), &v, options.params ? options.params : VVoid::get()};

			METHOD_FRAME_ACTION(*junction->method, options.r->method_frame, junction->self, {
				frame.store_params(params, 3);
				options.r->call(frame);
				return frame.result().as_string();
			});
		}
	}

	options.key=key;
	return *v.get_json_string(options);
}

static void _string(Request& r, MethodParams& params) {
	Json_options json(&r);

	if(params.count() == 2)
		if(HashStringValue* options=params.as_hash(1)) {
			json.params=&params[1];
			HashStringValue* methods=new HashStringValue();
			int valid_options=0;
			HashStringValue* vvalue;
			for(HashStringValue::Iterator i(*options); i; i.next() ){
				String::Body key=i.key();
				Value* value=i.value();
				if(key == "skip-unknown"){
					json.skip_unknown=r.process(*value).as_bool();
					valid_options++;
				} else if(key == "one-line"){
					json.one_line=r.process(*value).as_bool();
					valid_options++;
				} else if(key == "date" && value->is_string()){
					const String& svalue=value->as_string();
					if(!json.set_date_format(svalue))
						throw Exception(PARSER_RUNTIME, &svalue, "must be 'sql-string', 'gmt-string', 'iso-string' or 'unix-timestamp'");
					valid_options++;
				} else if(key == "indent"){
					if(value->is_string()){
						json.indent=value->as_string().cstr();
						json.json_string_recoursion=strlen(json.indent);
					} else json.indent=r.process(*value).as_bool() ? "" : NULL;
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
				} else if(key == "void" && value->is_string()){
					const String& svalue=value->as_string();
					if(!json.set_void_format(svalue))
						throw Exception(PARSER_RUNTIME, &svalue, "must be 'string' or 'null'");
					valid_options++;
#ifdef XML
				} else if(key == "xdoc" && (vvalue = value->get_hash())){
					json.xdoc_options=new XDocOutputOptions();
					json.xdoc_options->append(r, vvalue);
					valid_options++;
#endif
				} else if(Junction* junction=value->get_junction()){
					if(!junction->method || !junction->method->params_names || junction->method->params_count != 3)
						throw Exception(PARSER_RUNTIME, 0, "$.%s must be parser method with 3 parameters", key.cstr());
					methods->put(key, value);
					valid_options++;
				}
			}

			if(valid_options!=options->count())
				throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);

			// special handling for $._default 
			if(VHashBase* vhash=static_cast<VHashBase*>(params[1].as(VHASH_TYPE)))
				if(Value* value=vhash->get_default()) {
					if(!value->is_string()){
						Junction* junction=value->get_junction();
						if(!junction || !junction->method || !junction->method->params_names || junction->method->params_count != 3)
							throw Exception(PARSER_RUNTIME, 0, "$._default must be string or parser method with 3 parameters");
					}
					json.default_method=value;
				}

			if(methods->count())
				json.methods=methods;
		}

	const String& result_string=value_json_string(String::Body(), r.process(params[0]), json);
	String::Body result_body=result_string.cstr_to_string_body_untaint(String::L_JSON, r.connection(false), &r.charsets);
	if(json.one_line){
		char *result=result_body.cstrm();
		for(char *c=result;*c;c++)
			if(*c=='\n')
				*c=' ';
		result_body=result;
	}
	r.write(*new String(result_body, String::L_AS_IS));
}

// constructor

MJson::MJson(): Methoded("json") {
	add_native_method("parse", Method::CT_STATIC, _parse, 1, 2);

	add_native_method("string", Method::CT_ANY, _string, 1, 2);
}
