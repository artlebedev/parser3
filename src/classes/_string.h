/*
  $Id: _string.h,v 1.2 2001/03/10 11:03:46 paf Exp $
*/

#ifndef _STRING_H
#define _STRING_H

#include "pa_vclass.h"

extern VClass *string_class; // global string class [^length[] & co]
void initialize_string_class(Pool& pool, VClass& vclass);

#endif
