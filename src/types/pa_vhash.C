/** @file
	Parser: @b hash class.

	Copyright (c) 2001-2023 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_vhash.h"
#include "pa_vfile.h"

volatile const char * IDENT_PA_VHASH_C="$Id: pa_vhash.C,v 1.15 2023/09/26 20:49:12 moko Exp $" IDENT_PA_VHASH_H;

VFile* VHash::as_vfile(String::Language /*lang*/, const Request_charsets * /*charsets*/){
	return new VFile(fhash);
}

void VHash::extract_default(){
	if( (_default=fhash.get(Symbols::_DEFAULT_SYMBOL) ) )
		fhash.remove(Symbols::_DEFAULT_SYMBOL);
}
