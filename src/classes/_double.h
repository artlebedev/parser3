/*
  $Id: _double.h,v 1.2 2001/03/10 11:03:46 paf Exp $
*/

#ifndef _DOUBLE_H
#define _DOUBLE_H

#include "pa_vclass.h"

extern VClass *double_class; // global double class [^length[] & co]
void initialize_double_class(Pool& pool, VClass& vclass);

#endif
