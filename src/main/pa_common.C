/*
  $Id: pa_common.C,v 1.1 2001/01/29 20:46:22 paf Exp $
*/

#ifdef HAVE_CONFIG_H
#	include "pa_config.h"
#endif

#include <stdio.h>

#include "pa_common.h"

#ifdef WIN32

int __vsnprintf(char *b, size_t s, const char *f, va_list l) {
	int r=_vsnprintf(b, --s, f, l);
	b[s]=0;
	return r;
}
int __snprintf(char *b, size_t s, const char *f, ...) {
	va_list l;
    va_start(l, f);
    int r=__vsnprintf(b, s, f, l);
    va_end(l);
	return r;
}

#endif
