/** @file
	Parser: image parser class decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _image.h,v 1.2 2001/04/17 19:00:26 paf Exp $
*/

#ifndef CLASSES_IMAGE_H
#define CLASSES_IMAGE_H

#include "pa_vclass.h"

extern VStateless_class *image_class; // global image class [^length[] & co]
void initialize_image_class(Pool& pool, VStateless_class& vclass);

#endif
