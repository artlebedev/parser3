/** @file
	Parser: @b response class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VRESPONSE_C="$Date: 2002/10/25 08:05:05 $";

#include "pa_vresponse.h"
#include "pa_charsets.h"
#include "pa_charset.h"
#include "pa_vstring.h"
#include "pa_vdate.h"

Value *VResponse::get_element(const String& aname, Value *aself, bool looking_up) {
	// $charset
	if(aname==CHARSET_NAME)
		return NEW VString(pool().get_client_charset().name());

	// $method
	if(Value *result=VStateless_object::get_element(aname, aself, looking_up))
		return result;
	
	// $field
	return static_cast<Value *>(ffields.get(aname.change_case(pool(), String::CC_LOWER)));
}

bool VResponse::put_element(const String& aname, Value *avalue, bool /*replace*/) { 
	// guard charset change
	if(aname==CHARSET_NAME)
		pool().set_client_charset(charsets->get_charset(avalue->as_string()));
	else
		ffields.put(aname.change_case(pool(), String::CC_LOWER), avalue);

	return true;
}

// helper funcs

static size_t date_attribute(const VDate& vdate, char *buf, size_t buf_size) {
    const char month_names[12][4]={
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    const char days[7][4]={
		"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	time_t when=vdate.get_time();
	struct tm *tms=gmtime(&when);
	if(!tms)
		throw Exception(0,
			0,
			"bad time in attribute value (seconds from epoch=%ld)", when);
	return snprintf(buf, MAX_STRING, "%s, %.2d-%s-%.4d %.2d:%.2d:%.2d GMT", 
		days[tms->tm_wday],
		tms->tm_mday,month_names[tms->tm_mon],tms->tm_year+1900,
		tms->tm_hour,tms->tm_min,tms->tm_sec);
}
static void append_attribute_meaning(String& result,
									 Value& value, String::Untaint_lang lang,
									 bool forced=false) {
	if(const String *string=value.get_string())
		result.append(string->join_chains(result.pool(), 0), lang, forced);
	else
		if(Value *vdate=value.as(VDATE_TYPE, false)) {
			char *buf=(char *)result.malloc(MAX_STRING);
			size_t size=date_attribute(*static_cast<VDate *>(vdate), 
				buf, MAX_STRING);

			result.APPEND_CLEAN(buf, size, "converted from date", 0);
		} else
			throw Exception("parser.runtime",
				&result,
				"trying to append here neither string nor date (%s)",
					value.type());
}
#ifndef DOXYGEN
struct Attributed_meaning_info {
	String *header; // header line being constructed
	String::Untaint_lang lang; // language in which to append to that line
};
#endif
static void append_attribute_subattribute(const Hash::Key& akey, Hash::Val *avalue, 
										  void *info) {
	if(akey==VALUE_NAME)
		return;

	Attributed_meaning_info& ami=*static_cast<Attributed_meaning_info *>(info);

	// ...; charset=windows1251
	*ami.header << "; ";
	ami.header->append(akey, ami.lang);
	*ami.header << "=";
	append_attribute_meaning(*ami.header, *static_cast<Value *>(avalue), ami.lang);
}
const String& attributed_meaning_to_string(Value& meaning, 
										   String::Untaint_lang lang) {
	String &result=*new(meaning.pool()) String(meaning.pool());
	if(Hash *hash=meaning.get_hash(0)) {
		// $value(value) $subattribute(subattribute value)
		if(Value *value=static_cast<Value *>(hash->get(*value_name)))
			append_attribute_meaning(result, *value, lang, true);

		Attributed_meaning_info attributed_meaning_info={&result, lang};
		hash->for_each(append_attribute_subattribute, &attributed_meaning_info);
	} else // result value
		append_attribute_meaning(result, meaning, lang, true);

	return result;
}
