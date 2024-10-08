/** @file
	Parser: @b array class.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_varray.h"
#include "pa_vfile.h"
#include "pa_request.h"

volatile const char * IDENT_PA_VARRAY_C="$Id: pa_varray.C,v 1.4 2024/10/08 00:02:42 moko Exp $" IDENT_PA_VARRAY_H;

// Explicit instantiation for Visual Studio
template class SparseArray<Value*>;

template<typename T> void SparseArray<T>::fit(size_t index){
	if(index > pa_array_limit)
		throw Exception(PARSER_RUNTIME, 0, "array index %d exceeds the $MAIN:LIMITS.max_array_size (%d)", index, pa_array_limit);
	Array<T>::fit(index);
}

HashStringValue& VArray::hash(){
	if(fhash==0){
		fhash=new HashStringValue();
		for(ArrayValue::Iterator i(farray); i; i.next()) {
			Value *v=i.value();
			if(v)
				fhash->put(i.key(), v);
		}
	}
	return *fhash;
}

const String* VArray::get_json_string(Json_options& options) {
	return  options.array == Json_options::A_COMPACT ? options.array_compact_json_string(&array()) :
		options.array == Json_options::A_ARRAY   ? options.array_json_string(&array()) : Value::get_json_string(options);
}
