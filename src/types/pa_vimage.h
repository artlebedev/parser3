/** @file
	Parser: @b image parser type decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VIMAGE_H
#define PA_VIMAGE_H

static const char* IDENT_VIMAGE_H="$Date: 2003/11/20 15:35:32 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VIMAGE_TYPE "image"
#define EXIF_ELEMENT_NAME "exif"

// forwards

class gdImage;

/// simple gdImage-based font storage & text output 
class Font: public PA_Object {
public:
	
	const static int letter_spacing;
	int height;	    ///< Font heigth
	int monospace;	    ///< Default char width
	int spacebarspace; ///< spacebar width
	gdImage* ifont;
	const String& alphabet;
	
	Font(//
		const String& aalphabet, 
		gdImage* aifont, int aheight, int amonospace, int aspacebarspace);

	//@{******************************** char **********************************	
	size_t index_of(char ch);
	int index_width(size_t index);
	void index_display(gdImage* image, int x, int y, size_t index);
	//@}
	//@{******************************* string *********************************
	int step_width(int index);
	//@}
	/// counts trailing letter_spacing, consider this OK. useful for contiuations
	int string_width(const String& s);
	void string_display(gdImage* image, int x, int y, const String& s);	
};

// externs

extern Methoded* image_class;

/** holds img attributes 
	and [image itself]
*/
class VImage: public VStateless_object {
public: // Value
	
	override const char* type() const { return VIMAGE_TYPE; }
	override VStateless_class *get_class() { return image_class; }

	/// VImage: true
	override bool as_bool() const { return true; }

	/// VImage: true
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return *new VBool(as_bool()); }

	/// VImage: method,field
	override Value* get_element(const String& aname, Value& aself, bool looking_up);

	/// VImage: field
	override bool put_element(const String& aname, Value* avalue, bool replace);

public: // usage

	void set(const String* src, int width, int height,
		gdImage* aimage,
		Value* aexif=0);

	HashStringValue& fields() { return ffields; }

public:

	gdImage* image; 
	Font* font;

private:

	HashStringValue ffields;
	Value* fexif;

};

#endif
