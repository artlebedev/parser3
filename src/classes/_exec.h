/*
	Parser: @b exec parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _exec.h,v 1.1 2001/04/09 09:48:15 paf Exp $
*/

#ifndef _EXEC_H
#define _EXEC_H

#include "pa_vclass.h"

extern VStateless_class *mail_class;
void initialize_op_class(Pool& pool, VStateless_class& vclass);

#endif
