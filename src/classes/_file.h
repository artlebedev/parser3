/** @file
	Parser: @b file parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _file.h,v 1.4 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_FILE_H
#define CLASSES_FILE_H

#include "pa_vstateless_class.h"

extern VStateless_class *file_class;
void initialize_file_class(Pool& pool, VStateless_class& vclass);

#endif
