/** @file
	Parser: @b DOM parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdom.h,v 1.3 2001/09/10 13:13:55 parser Exp $
*/

#ifndef PA_VDOM_H
#define PA_VDOM_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

#include <XalanTransformer/XalanTransformer.hpp>

extern Methoded *Dom_class;

/// value of type 'DOM'. implemented with @c DOM
class VDom : public VStateless_object {
public: // Value

	const char *type() const { return "dom"; }
	/// VDom: clone
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	/// @test XalanTransformer free somehow
	VDom(Pool& apool, XalanDocument* adocument) : VStateless_object(apool, *Dom_class), 
		ftransformer(new XalanTransformer),
		fdocument(adocument) {
	}

	XalanTransformer& get_transformer() {return *ftransformer; }

	XalanDocument* get_document() { return fdocument; }
	void set_document(XalanDocument* adocument) { fdocument=adocument; }

private:

	XalanTransformer *ftransformer;
	XalanDocument* fdocument;

};

#endif
