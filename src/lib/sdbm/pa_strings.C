/** @file
	Parser: implementation of apr functions.

	Copyright (c) 2000-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#include "pa_strings.h"
#include "pa_memory.h"
#include "pa_common.h"

volatile const char * IDENT_PA_STRINGS_C="$Id: pa_strings.C,v 1.9 2024/12/26 02:16:31 moko Exp $";

char *pa_pstrcat(const char *a, const char *b){
    return pa_strcat(a, b);
}

void* pa_sdbm_malloc(unsigned int size){
        return pa_malloc(size);
}
