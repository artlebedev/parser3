/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vxdoc.C,v 1.8 2001/12/27 19:57:10 paf Exp $
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vxdoc.C,v 1.8 2001/12/27 19:57:10 paf Exp $"; 

#include "pa_vxdoc.h"

#include <XalanDOM/XalanDocumentType.hpp>
#include <XalanDOM/XalanElement.hpp>

void VXdoc_destructor(void *vxdoc) {
	//_asm int 3;
	static_cast<VXdoc *>(vxdoc)->~VXdoc();
}

/// VXdoc: $CLASS,$method
Value *VXdoc::get_element(const String& name) { 
	// $CLASS,$method
	if(Value *result=VStateless_object::get_element(name))
		return result;

	// fields
	GdomeDocument *document=get_document(pool(), &name);
	GdomeException exc;

	if(name=="doctype") {
		// readonly attribute DocumentType doctype;
		return NEW VXnode(pool(), gdome_doc_doctype(document, &exc), false);
	} else if(name=="implementation") {
		// readonly attribute DOMImplementation implementation;
		return 0;
	} else if(name=="documentElement") {
		// readonly attribute Element documentElement;
		return NEW VXnode(pool(), document.gdome_doc_documentElement(document, &exc), false);
	} 	

	return 0;
}

#endif
