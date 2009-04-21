/** @file
	Parser: Value class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VALUE_C="$Date: 2009/04/21 09:26:08 $";

#include "pa_value.h"
#include "pa_vstateless_class.h"
#include "pa_vmethod_frame.h"
#include "pa_vdate.h"
#include "pa_vobject.h"

// globals

const String name_name(NAME_NAME);

const String value_name(VALUE_NAME);
const String expires_name(EXPIRES_NAME);
const String content_type_name(CONTENT_TYPE_NAME);

// methods

VObject* Value::set_derived(VObject* /*aderived*/) { return 0; }

Junction* Value::get_junction() { return 0; }

Value* Value::base_object() { return bark("is '%s', it has no base object"); }

Value* Value::get_element(const String& /*aname*/, Value& /*aself*/, bool /*looking_up*/) {
	return bark("is '%s', it has no elements");
}


VFile* Value::as_vfile(String::Language /*lang*/, const Request_charsets* /*charsets*/) { 
	bark("is '%s', it does not have file value"); return 0;
}

/// call this before invoking to ensure proper actual numbered params count
void Method::check_actual_numbered_params(Value& self, 
					  MethodParams* actual_numbered_params) const {

	int actual_count=actual_numbered_params?actual_numbered_params->count():0;
	if(actual_count<min_numbered_params_count) // not proper count? bark
		throw Exception(PARSER_RUNTIME,
			0,
			"native method of %s (%s) accepts minimum %d parameter(s) (%d present)", 
				self.get_class()->name_cstr(),
				self.type(),
				min_numbered_params_count,
				actual_count);

}

// attributed meaning

static String::C date_attribute(const VDate& vdate) {
	time_t when=vdate.get_time();
	struct tm *tms=gmtime(&when);
	if(!tms)
		throw Exception(DATE_RANGE_EXCEPTION_TYPE,
			0,
			"bad time in attribute value (seconds from epoch=%u)", when);
	return date_gmt_string(tms);
}

static void append_attribute_meaning(String& result,
				     Value& value, String::Language lang, bool forced) {
	if(const String* string=value.get_string())
		result.append(*string, lang, forced);
	else
		if(Value* vdate=value.as(VDATE_TYPE, false)) {
			String::C attribute=date_attribute(static_cast<VDate&>(*vdate));

			result.append_help_length(attribute.str, attribute.length, String::L_CLEAN);
		} else
			throw Exception(PARSER_RUNTIME,
				&result,
				"trying to append here neither string nor date (%s)",
					value.type());
}
#ifndef DOXYGEN
struct Attributed_meaning_info {
	String* header; // header line being constructed
	String::Language lang; // language in which to append to that line
	bool forced; // do they force that lang?
	bool allow_bool; // allow bool types during print attributes
};
#endif
static void append_attribute_subattribute(HashStringValue::key_type akey, 
					  HashStringValue::value_type avalue, 
					  Attributed_meaning_info *info) {
	if(akey==VALUE_NAME)
		return;

	// ...; charset=windows1251
	*info->header << "; ";
	info->header->append(String(akey, String::L_TAINTED), info->lang, info->forced);
	if(!info->allow_bool || !avalue->is_bool()){
		*info->header << "=";
		append_attribute_meaning(*info->header, *avalue, info->lang, info->forced);
	}
}
const String& attributed_meaning_to_string(Value& meaning, 
					   String::Language lang, bool forced, bool allow_bool) {
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
