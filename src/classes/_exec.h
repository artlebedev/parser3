/*
	Parser: @b exec parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _exec.h,v 1.2 2001/04/09 10:08:28 paf Exp $
*/

#ifndef _EXEC_H
#define _EXEC_H

#include "pa_vclass.h"

extern VStateless_class *exec_class;
void initialize_exec_class(Pool& pool, VStateless_class& vclass);

#endif
