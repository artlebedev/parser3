/** @file
	Parser: Value class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_value.h"
#include "pa_vstateless_class.h"
#include "pa_vmethod_frame.h"
#include "pa_vdate.h"
#include "pa_vobject.h"
#include "pa_request.h"


volatile const char * IDENT_PA_VALUE_C="$Id: pa_value.C,v 1.47 2020/12/15 17:10:38 moko Exp $" IDENT_PA_VALUE_H IDENT_PA_PROPERTY_H;

// globals

const String name_name(NAME_NAME);

const String value_name(VALUE_NAME);
const String expires_name(EXPIRES_NAME);
const String content_type_name(CONTENT_TYPE_NAME);

// methods

Junction* Value::get_junction() { return 0; }

Value* Value::get_element(const String& /*aname*/) {
	return bark("element can not be fetched from %s");
}

VFile* Value::as_vfile(String::Language /*lang*/, const Request_charsets* /*charsets*/) { 
	bark("is '%s', it does not have file value"); return 0;
}

// Should be synced with MethodParams::as_hash
HashStringValue* Value::as_hash(const char* name) {
	if(get_junction())
		throw Exception(PARSER_RUNTIME, 0, "%s must not be code", name ? name : "options");
	if(!is_defined()) // empty hash is not defined, but we don't need it anyway
		return 0;
	if(HashStringValue* result=get_hash())
		return result;
	if(is_string() && get_string()->trim().is_empty())
		return 0;
	throw Exception(PARSER_RUNTIME, 0, "%s must have hash representation", name ? name : "options");
}

const String* Value::get_json_string(Json_options& options) {
	if(HashStringValue* hash=get_hash())
		return options.hash_json_string(hash);

	if(!options.skip_unknown)
		throw Exception(PARSER_RUNTIME, 0, "Unsupported value's type (%s)", type());

	return new String("null");
}

const String* Value::default_method_2_json_string(Value& default_method, Json_options& options){
	if(default_method.is_string()){
		// specified as string with method name
		const String& method_name=*default_method.get_string();
		Method* method=this->get_class()->get_method(method_name);
		if(!method) {
			// class/object does not have method with specified name so serialize it as hash (default)
			return options.hash_json_string(get_hash());
		}

		Value *params[]={new VString(*new String(options.key, String::L_JSON)), options.params ? options.params : VVoid::get()};

		METHOD_FRAME_ACTION(*method, options.r->method_frame, *this,{
			frame.store_params(params, 2);
			options.r->call(frame);
			return &frame.result().as_string();
		});
	} else {
		// specified as method-junction
		Junction* junction=default_method.get_junction();

		Value *params[]={new VString(*new String(options.key, String::L_JSON)), this, options.params ? options.params : VVoid::get()};

		METHOD_FRAME_ACTION(*junction->method, options.r->method_frame, junction->self,{
			frame.store_params(params, 3);
			options.r->call(frame);
			return &frame.result().as_string();
		});
	}
}

/// call this before invoking to ensure proper actual numbered params count
void Method::check_actual_numbered_params(Value& self, MethodParams* actual_numbered_params) const {
	int actual_count=actual_numbered_params ? actual_numbered_params->count() : 0;
	if(actual_count<min_numbered_params_count || actual_count>max_numbered_params_count)
		throw Exception(PARSER_RUNTIME, name, "native method of '%s' accepts %s %d parameter(s) (%d present)", 
			self.type(),
			actual_count<min_numbered_params_count ? "minimum" : "maximum",
			actual_count<min_numbered_params_count ? min_numbered_params_count : max_numbered_params_count,
			actual_count);
}

// attributed meaning

static void append_attribute_meaning(String& result, Value& value, String::Language lang, bool forced) {
	if(const String* string=value.get_string())
		result.append(*string, lang, forced);
	else
		if(Value* vdate=value.as(VDATE_TYPE)) {
			result << *static_cast<VDate&>(*vdate).get_gmt_string();
		} else
			throw Exception(PARSER_RUNTIME, &result, "trying to append here neither string nor date (%s)", value.type());
}

#ifndef DOXYGEN
struct Attributed_meaning_info {
	String* header;        // header line being constructed
	String::Language lang; // language in which to append to that line
	bool forced;           // do they force that lang?
	bool allow_bool;       // allow bool types during print attributes
};
#endif

static void append_attribute_subattribute(HashStringValue::key_type akey, HashStringValue::value_type avalue, Attributed_meaning_info *info) {
	if(akey==VALUE_NAME)
		return;

	if(avalue->is_bool() && (!info->allow_bool || avalue->as_bool()==false))
		return;

	// ...; charset=windows1251
	*info->header << "; ";
	info->header->append(String(akey, String::L_TAINTED), info->lang, info->forced);
	if(!avalue->is_bool()) {
		if( akey==content_disposition_filename_name ) {
			*info->header << "=\"";
			append_attribute_meaning(*info->header, *avalue, info->lang, info->forced);
			*info->header << "\"";
		} else {
			*info->header << "=";
			append_attribute_meaning(*info->header, *avalue, info->lang, info->forced);
		}
	}
}
const String& attributed_meaning_to_string(Value& meaning, String::Language lang, bool forced, bool allow_bool) {
	String& result=*new String;
	if(HashStringValue *hash=meaning.get_hash()) {
		// $value(value) $subattribute(subattribute value)
		if(Value* value=hash->get(value_name))
			append_attribute_meaning(result, *value, lang, forced);

		Attributed_meaning_info attributed_meaning_info={&result, lang, false, allow_bool};
		hash->for_each<Attributed_meaning_info*>(append_attribute_subattribute, &attributed_meaning_info);
	} else // result value
		append_attribute_meaning(result, meaning, lang, forced);

	return result;
}
