/** @file
	Parser: @b mail parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _mail.h,v 1.3 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_MAIL_H
#define CLASSES_MAIL_H

#include "pa_vclass.h"

extern VStateless_class *mail_class; // global mail class [^length[] & co]
void initialize_mail_class(Pool& pool, VStateless_class& vclass);

#endif
