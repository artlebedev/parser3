/** @file
	Parser: @b form parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _form.h,v 1.3 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_FORM_H
#define CLASSES_FORM_H

#include "pa_vstateless_class.h"

extern VStateless_class *form_base_class;
void initialize_form_base_class(Pool& pool, VStateless_class& vclass);

#endif
