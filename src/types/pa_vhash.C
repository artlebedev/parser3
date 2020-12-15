/** @file
	Parser: @b hash class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_vhash.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VHASH_C="$Id: pa_vhash.C,v 1.14 2020/12/15 17:10:40 moko Exp $" IDENT_PA_VHASH_H;

VFile* VHash::as_vfile(String::Language /*lang*/, const Request_charsets * /*charsets*/){
	return new VFile(fhash);
}

void VHash::extract_default(){
	if( (_default=fhash.get(Symbols::_DEFAULT_SYMBOL) ) )
		fhash.remove(Symbols::_DEFAULT_SYMBOL);
}
