/*
  $Id: pa_types.h,v 1.2 2001/01/29 15:56:04 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#ifdef HAVE_CONFIG_H
#include "pa_config.h"
#endif


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
