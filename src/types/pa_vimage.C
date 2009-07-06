/** @file
	Parser: @b image parser type.

	Copyright(c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_VIMAGE_C="$Date: 2009/07/06 12:10:09 $";

#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vstring.h"
#include "gif.h"
#include "pa_vbool.h"


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

Value& VImage::as_expr_result(bool /*return_string_as_is=false*/) {
	return VBool::get(as_bool());
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

const VJunction* VImage::put_element(Value& /*aself*/, const String& aname, Value* avalue, bool /*replace*/) {
	ffields.put(aname, avalue);

	if(fimage) {
		if(aname=="line-width") {
			fimage->SetLineWidth(min(max(avalue->as_int(), 1), 10));
		} else if(aname=="line-style") {
			const String& sline_style=avalue->as_string();
			fimage->SetLineStyle(sline_style.length()?sline_style.taint_cstr(String::L_AS_IS):0);
		}
	}

	return PUT_ELEMENT_REPLACED_ELEMENT;	
}
