/** @file
	Parser: @b DOM parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdom.h,v 1.2 2001/09/07 12:46:19 parser Exp $
*/

#ifndef PA_VDOM_H
#define PA_VDOM_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

#include <XalanTransformer/XalanTransformer.hpp>

extern Methoded *Dom_class;

/// value of type 'DOM'. implemented with @c DOM
class VDOM : public VStateless_object {
public: // Value

	const char *type() const { return "dom"; }
	/// VDOM: clone
	Value *as_expr_result(bool return_string_as_is=false) { return this; }

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	/// @test XalanTransformer free somehow
	VDOM(Pool& apool, XalanParsedSource* aparsedSource) : VStateless_object(apool, *Dom_class), 
		theXalanTransformer(new XalanTransformer),
		fparsedSource(aparsedSource) {
	}

	XalanTransformer& getXalanTransformer() {return *theXalanTransformer; }

	XalanParsedSource* getParsedSource() { return fparsedSource; }
	void setParsedSource(XalanParsedSource* aparsedSource) { fparsedSource=aparsedSource; }

private:

	XalanTransformer *theXalanTransformer;
	XalanParsedSource* fparsedSource;

};

#endif
