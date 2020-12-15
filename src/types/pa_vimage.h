/** @file
	Parser: @b image parser type decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VIMAGE_H
#define PA_VIMAGE_H

#define IDENT_PA_VIMAGE_H "$Id: pa_vimage.h,v 1.61 2020/12/15 17:10:41 moko Exp $"

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_charset.h"

// defines

#define VIMAGE_TYPE "image"

// forwards

class gdImage;

/// simple gdImage-based font storage & text output 
class Font: public PA_Object {
public:
	
	int letterspacing;
	int height;		///< Font heigth
	int monospace;		///< Default char width
	int spacebarspace;	///< spacebar width
	gdImage* ifont;
	const String& alphabet;
	
	Font(
		Charset& acharset, const String& aalphabet, 
		gdImage* aifont, int aheight, int amonospace, int aspacebarspace, int aletterspacing);

	//@{******************************** char **********************************
	size_t index_of(char ch);
	size_t index_of(XMLCh ch);
	int index_width(size_t index);
	void index_display(gdImage& image, int x, int y, size_t index);
	//@}
	//@{******************************* string *********************************
	int step_width(int index);
	//@}
	/// counts trailing letter_spacing, consider this OK. useful for contiuations
	int string_width(const String& s);
	void string_display(gdImage& image, int x, int y, const String& s);

private:
	Charset& fsource_charset;
	Hash<XMLCh, size_t> fletter2index;
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
	override Value& as_expr_result();

	/// VImage: method,field
	override Value* get_element(const String& aname);

	/// VImage: field
	override const VJunction* put_element(const String& name, Value* value);

	/// VImage: fields
	HashStringValue *get_hash() { return &ffields; }

public: // usage

	void set(const String* src, int width, int height, gdImage* aimage, Value* aexif=0, Value* axmp=0);

	HashStringValue& fields() { return ffields; }

public:

	gdImage& image() {
		if(!fimage)
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized image object");
		return *fimage;
	}

	void set_font(Font* afont) { ffont=afont; }

	Font& font() {
		if(!ffont)
			throw Exception(PARSER_RUNTIME, 0, "set the font first");
		return *ffont;
	}

private:

	gdImage* fimage;
	Font* ffont;
	HashStringValue ffields;

};

#endif
