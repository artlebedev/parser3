/** @file
	Parser: @b image parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vimage.C,v 1.7 2001/04/26 15:01:51 paf Exp $
*/

#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vstring.h"

void VImage::set(const String *src, int width, int height,
				 gdImage* aimage) {
	image=aimage;

	// $src
	if(src)
		ffields.put(String(pool(), "src"), NEW VString(*src));
	// $width
	if(width)
		ffields.put(String(pool(), "width"), NEW VInt(pool(), width));
	// $height
	if(height)
		ffields.put(String(pool(), "height"), NEW VInt(pool(), height));

	// defaults
	// $border[0] 
	ffields.put(String(pool(), "border"), NEW VInt(pool(), 0));
}
