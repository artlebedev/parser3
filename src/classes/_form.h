/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _form.h,v 1.1 2001/03/13 17:54:11 paf Exp $
*/

#ifndef _FORM_H
#define _FORM_H

#include "pa_vstateless_class.h"

extern VStateless_class *form_base_class;
void initialize_form_base_class(Pool& pool, VStateless_class& vclass);

#endif
