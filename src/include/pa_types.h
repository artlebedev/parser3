/*
  $Id: pa_types.h,v 1.7 2001/03/10 11:03:48 paf Exp $
*/

#ifndef PA_TYPES_H
#define PA_TYPES_H

#ifdef HAVE_CONFIG_H
#include "pa_config.h"
#endif

#ifdef WIN32
#define open _open
#define close _close
#define read _read
#define stat _stat
#endif

// for now: used in Exception::fcomment
#define MAX_STRING 5*0x400

#define STATIC_NAME "STATIC"

#undef uint
typedef unsigned int uint;

#ifndef NO_STRING_ORIGIN
struct Origin {
	char *file;  // macros file name | load file name | sql query text
	uint line; // file line no | record no
};
#endif

#endif
