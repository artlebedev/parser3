/** @file
	Parser: @b image parser type decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vimage.h,v 1.23 2002/02/08 07:27:53 paf Exp $
*/

#ifndef PA_VIMAGE_H
#define PA_VIMAGE_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VIMAGE_TYPE "image"

// forwards

class gdImage; class Font;

// externs

extern Methoded *image_class;

/** holds img attributes 
	and [image itself]
*/
class VImage : public VStateless_object {
public: // Value
	
	const char *type() const { return VIMAGE_TYPE; }

	/// VImage: true
	bool as_bool() const { return true; }

	/// VImage: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VImage: method,field
	Value *VImage::get_element(const String& aname) {
		// $method
		if(Value *result=VStateless_object::get_element(aname))
			return result;

		// $src, $size
		return static_cast<Value *>(ffields.get(aname));
	}

	/// VImage: field
	void put_element(const String& aname, Value *avalue);

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VImage::VImage(Pool& apool) : VStateless_object(apool, *image_class),
		ffields(apool),
		image(0), font(0) {
	}

	void set(const String *src, int width, int height,
		gdImage *aimage=0);

	Hash fields() const { return ffields; }

public:

	gdImage *image; Font *font;

private:

	Hash ffields;

};

#endif
