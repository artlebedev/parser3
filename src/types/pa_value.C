/** @file
	Parser: Value class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VALUE_C="$Date: 2004/09/01 09:16:58 $";

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
		throw Exception("parser.runtime",
			0,
			"native method of %s (%s) accepts minimum %d parameter(s) (%d present)", 
				self.get_class()->name_cstr(),
				self.type(),
				min_numbered_params_count,
				actual_count);

}

Junction::Junction(Value& aself,
	const Method* amethod,
	VMethodFrame* amethod_frame,
	Value* arcontext,
	WContext* awcontext,
	ArrayOperation* acode): self(aself),
	method(amethod),
	method_frame(amethod_frame),
	rcontext(arcontext),
	wcontext(awcontext),
	code(acode) {
	if(wcontext)
		wcontext->attach_junction(this);
}

void Junction::reattach(WContext *new_wcontext) {
	if(new_wcontext) {
		assert(wcontext!=new_wcontext);
		wcontext=new_wcontext;
		wcontext->attach_junction(this);
	} else {
		method_frame=0;
		rcontext=0;
		wcontext=0;
	}
}

// attributed meaning

static String::C date_attribute(const VDate& vdate) {
	/// http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.3
	const char month_names[12][4]={
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	const char days[7][4]={
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
			
	time_t when=vdate.get_time();
	struct tm *tms=gmtime(&when);
	if(!tms)
		throw Exception(0,
			0,
			"bad time in attribute value (seconds from epoch=%u)", when);

	char *buf=new(PointerFreeGC) char[MAX_STRING];
	return String::C(buf, 
		snprintf(buf, MAX_STRING, "%s, %.2d %s %.4d %.2d:%.2d:%.2d GMT", 
		days[tms->tm_wday],
		tms->tm_mday,month_names[tms->tm_mon],tms->tm_year+1900,
		tms->tm_hour,tms->tm_min,tms->tm_sec));
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
			throw Exception("parser.runtime",
				&result,
				"trying to append here neither string nor date (%s)",
					value.type());
}
#ifndef DOXYGEN
struct Attributed_meaning_info {
	String* header; // header line being constructed
	String::Language lang; // language in which to append to that line
	bool forced; // do they force that lang?
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
	*info->header << "=";
	append_attribute_meaning(*info->header, *avalue, info->lang, info->forced);
}
const String& attributed_meaning_to_string(Value& meaning, 
					   String::Language lang, bool forced) {
	String& result=*new String;
	if(HashStringValue *hash=meaning.get_hash()) {
		// $value(value) $subattribute(subattribute value)
		if(Value* value=hash->get(value_name))
			append_attribute_meaning(result, *value, lang, forced);

		Attributed_meaning_info attributed_meaning_info={&result, lang, false};
		hash->for_each(append_attribute_subattribute, &attributed_meaning_info);
	} else // result value
		append_attribute_meaning(result, meaning, lang, forced);

	return result;
}
