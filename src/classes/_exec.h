/*
	Parser: @b exec parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _exec.h,v 1.3 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_EXEC_H
#define CLASSES_EXEC_H

#include "pa_vclass.h"

extern VStateless_class *exec_class;
void initialize_exec_class(Pool& pool, VStateless_class& vclass);

#endif
