/** @file
	Parser: mail parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _mail.h,v 1.1 2001/04/07 10:34:39 paf Exp $
*/

#ifndef _MAIL_H
#define _MAIL_H

#include "pa_vclass.h"

extern VStateless_class *mail_class; // global mail class [^length[] & co]
void initialize_mail_class(Pool& pool, VStateless_class& vclass);

#endif
