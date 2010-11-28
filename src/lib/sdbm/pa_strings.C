/** @file
	Parser: implementation of apr functions.

	Copyright(c) 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2010/11/28 13:58:03 $";

#include "pa_strings.h"
#include "pa_memory.h"

/** this is used to cache lengths in pa_pstrcat */
#define MAX_SAVED_LENGTHS  6

char *pa_pstrcat(pa_pool_t *p, ...)
{
    char *cp, *argp, *res;
    pa_size_t saved_lengths[MAX_SAVED_LENGTHS];
    int nargs = 0;

    /* Pass one --- find length of required string */

    pa_size_t len = 0;
    va_list adummy;

    va_start(adummy, p);

    while ((cp = va_arg(adummy, char *)) != NULL) {
        pa_size_t cplen = strlen(cp);
        if (nargs < MAX_SAVED_LENGTHS) {
            saved_lengths[nargs++] = cplen;
        }
        len += cplen;
    }

    va_end(adummy);

    /* Allocate the required string */

    res = (char *) pa_malloc_atomic(len + 1);
    cp = res;

    /* Pass two --- copy the argument strings into the result space */

    va_start(adummy, p);

    nargs = 0;
    while ((argp = va_arg(adummy, char *)) != NULL) {
        if (nargs < MAX_SAVED_LENGTHS) {
            len = saved_lengths[nargs++];
        }
        else {
            len = strlen(argp);
        }
 
        memcpy(cp, argp, len);
        cp += len;
    }

    va_end(adummy);

    /* Return the result string */

    *cp = '\0';

    return res;
}

