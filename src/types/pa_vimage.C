/** @file
	Parser: @b image parser type.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_VIMAGE_C="$Date: 2002/08/13 13:02:41 $";

#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vstring.h"
#include "gif.h"

void VImage::set(const String *src, int width, int height,
				 gdImage* aimage) {
	image=aimage;

	// $src
	if(src)
		ffields.put(*NEW String(pool(), "src"), NEW VString(*src));
	// $width
	if(width)
		ffields.put(*NEW String(pool(), "width"), NEW VInt(pool(), width));
	// $height
	if(height)
		ffields.put(*NEW String(pool(), "height"), NEW VInt(pool(), height));

	// defaults
	// $border(0)
	ffields.put(*NEW String(pool(), "border"), NEW VInt(pool(), 0));

	// internals, take a look at image.C append_attrib_pair before update
	// $line-width(1) 
	ffields.put(*NEW String(pool(), "line-width"), NEW VInt(pool(), 1));
}


bool VImage::put_element(const String& aname, Value *avalue, bool replace) {
	ffields.put(aname, avalue);

	if(image) {
		if(aname=="line-width") {
			image->SetLineWidth(min(max(avalue->as_int(), 1), 10));
		} else if(aname=="line-style") {
			const String sline_style=avalue->as_string();
			image->SetLineStyle(sline_style.size()?sline_style.cstr(String::UL_AS_IS):0);
		}
	}

	return true;	
}
