/** @file
	Parser: @b hash parser type impl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VHASH_C="$Date: 2003/07/24 11:31:26 $";

#include "pa_vhash.h"

// defines for globals

#define HASH_DEFAULT_ELEMENT_NAME "_default"

// globals

const String hash_default_element_name(HASH_DEFAULT_ELEMENT_NAME);
