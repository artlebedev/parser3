/** @file
	Parser: @b image parser type decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VIMAGE_H
#define PA_VIMAGE_H

static const char* IDENT_VIMAGE_H="$Date: 2002/11/22 16:16:34 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VIMAGE_TYPE "image"
#define EXIF_ELEMENT_NAME "exif"

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
	VStateless_class *get_class() { return image_class; }

	/// VImage: true
	bool as_bool() const { return true; }

	/// VImage: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VImage: method,field
	/*override*/ Value *get_element(const String& aname, Value& aself, bool looking_up);

	/// VImage: field
	/*override*/ bool put_element(const String& aname, Value *avalue, bool replace);

public: // usage

	VImage::VImage(Pool& apool) : VStateless_object(apool),
		ffields(apool),
		fexif(0), image(0), font(0) {
	}

	void set(const String *src, int width, int height,
		gdImage *aimage=0,
		Value *aexif=0);

	Hash& fields() { return ffields; }

public:

	gdImage *image; Font *font;

private:

	Hash ffields;
	Value *fexif;

};

#endif
