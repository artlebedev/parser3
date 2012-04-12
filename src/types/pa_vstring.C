/** @file
	Parser: @b string class.

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vstring.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VSTRING_C="$Id: pa_vstring.C,v 1.29 2012/04/12 22:44:47 moko Exp $" IDENT_PA_VSTRING_H;

VFile* VString::as_vfile(String::Language lang, const Request_charsets* charsets) {
	VFile& result=*new VFile;
	String::Body sbody=fstring->cstr_to_string_body_untaint(lang, 0, charsets);
	result.set(false/*not tainted*/, sbody.cstr(), sbody.length());
	return &result;
}

void VString::serialize(Serialization_data &data){
	if(!is_defined()){
		data = Serialization_data(SERIALIZED_STRING);
		return;
	}
	
	if (string().is_not_just_lang()){
		String::Cm cm = string().serialize(0);
		data = Serialization_data(SERIALIZED_STRING, cm.str, cm.length);
	} else {
		data = Serialization_data(SERIALIZED_STRING + (unsigned int)string().just_lang(), string().cstr(), string().length());
	}
}

VString *VString::deserialize(Serialization_data &data){
	String *result;
	
	if(data.flags==SERIALIZED_STRING){
		result = new String();
		if (data.length>0 && !result->deserialize(0, (void *)data.ptr, data.length))
			return 0;
	} else {
		// we can't use length from memcached as there can be '\0' inside
		String::Language lang=(String::Language)(data.flags-SERIALIZED_STRING);
		result = new String(data.ptr, lang);
	}

	return new VString(*result);
}
