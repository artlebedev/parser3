/** @file
	Parser: @b image parser type.

	Copyright(c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VIMAGE_C="$Date: 2004/03/01 13:22:25 $";

#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vstring.h"
#include "gif.h"

void VImage::set(const String* src, int width, int height,
				 gdImage* aimage,
				 Value* aexif) {
	fimage=aimage;
	fexif=aexif;

	// $src
	if(src)
		ffields.put(String::Body("src"), new VString(*src));
	// $width
	if(width)
		ffields.put(String::Body("width"), new VInt(width));
	// $height
	if(height)
		ffields.put(String::Body("height"), new VInt(height));
	// defaults
	// $border(0)
	ffields.put(String::Body("border"), new VInt(0));

	// internals, take a look at image.C append_attrib_pair before update
	// $line-width(1) 
	ffields.put(String::Body("line-width"), new VInt(1));
}


Value* VImage::get_element(const String& aname, Value& aself, bool looking_up) {
	// $method
	if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
		return result;

	// $exif
	if(aname==EXIF_ELEMENT_NAME)
		return fexif;

	// $src, $size
	return ffields.get(aname);
}

bool VImage::put_element(const String& aname, Value* avalue, bool /*replace*/) {
	ffields.put(aname, avalue);

	if(fimage) {
		if(aname=="line-width") {
			fimage->SetLineWidth(min(max(avalue->as_int(), 1), 10));
		} else if(aname=="line-style") {
			const String& sline_style=avalue->as_string();
			fimage->SetLineStyle(sline_style.length()?sline_style.cstr(String::L_AS_IS):0);
		}
	}

	return true;	
}
