/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

static const char* IDENT_VXDOC="$Date: 2002/08/01 11:41:25 $";

#include "pa_vxdoc.h"

void VXdoc_destructor(void *vxdoc) {
	//_asm int 3;
	static_cast<VXdoc *>(vxdoc)->~VXdoc();
}

/// VXdoc: $CLASS,$method
Value *VXdoc::get_element(const String& name) { 
	// up
	if(Value *result=VXnode::get_element(name))
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
