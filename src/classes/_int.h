/*
  $Id: _int.h,v 1.2 2001/03/10 11:03:46 paf Exp $
*/

#ifndef _INT_H
#define _INT_H

#include "pa_vclass.h"

extern VClass *int_class; // global int class [^length[] & co]
void initialize_int_class(Pool& pool, VClass& vclass);

#endif
