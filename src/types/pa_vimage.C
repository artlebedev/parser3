/** @file
	Parser: @b image parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vimage.C,v 1.8 2001/09/18 16:05:43 parser Exp $
*/

#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vstring.h"
#include "gif.h"

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
	// $border(0)
	ffields.put(String(pool(), "border"), NEW VInt(pool(), 0));
	// $line-width(1) 
	ffields.put(String(pool(), "line-width"), NEW VInt(pool(), 1));
}


void VImage::put_element(const String& aname, Value *avalue) {
	ffields.put(aname, avalue);

	if(image)
		if(aname=="line-width") {
			image->SetLineWidth(min(max(avalue->as_int(), 1), 10));
		} else if(aname=="line-style") {
			const String sline_style=avalue->as_string();
			image->SetLineStyle(sline_style.size()?sline_style.cstr(String::UL_AS_IS):0);
		}
}
