/** @file
	Parser: implementation of apr functions.

	Copyright (c) 2000-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_strings.h"
#include "pa_memory.h"
#include "pa_common.h"

volatile const char * IDENT_PA_STRINGS_C="$Id: pa_strings.C,v 1.10 2026/04/25 13:38:46 moko Exp $";

char *pa_pstrcat(const char *a, const char *b){
    return pa_strcat(a, b);
}

void* pa_sdbm_malloc(unsigned int size){
        return pa_malloc(size);
}
