/*
  $Id: pa_types.h,v 1.3 2001/01/29 20:10:32 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#ifdef HAVE_CONFIG_H
#include "pa_config.h"
#endif

#define MAX_STRING 2*0x400

#undef uint
typedef unsigned int uint;

class Origin {
public:
#ifndef NO_STRING_ORIGIN
	char *file;  // macros file name | load file name | sql query text
	uint line; // file line no | record no
#endif
};

#endif
