/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001, 2002 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://paf.design.ru)

	$Id: pa_vxdoc.C,v 1.10 2002/02/08 07:27:55 paf Exp $
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vxdoc.C,v 1.10 2002/02/08 07:27:55 paf Exp $"; 

#include "pa_vxdoc.h"

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
	GdomeDocument *document=get_document(&name);
	GdomeException exc;

	if(name=="doctype") {
		// readonly attribute DocumentType doctype;
		return NEW VXnode(pool(), (GdomeNode *)gdome_doc_doctype(document, &exc));
	} else if(name=="implementation") {
		// readonly attribute DOMImplementation implementation;
		return 0;
	} else if(name=="documentElement") {
		// readonly attribute Element documentElement;
		return NEW VXnode(pool(), (GdomeNode *)gdome_doc_documentElement(document, &exc));
	} 	

	return 0;
}

#endif
