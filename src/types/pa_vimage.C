/** @image
	Parser: @b image parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vimage.C,v 1.2 2001/04/10 14:00:04 paf Exp $
*/

#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vstring.h"

void VImage::set(const String& src, int width, int height) {
	// $src
	ffields.put(String(pool(), "src"), NEW VString(src));
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
