/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

static const char * const IDENT_VXDOC="$Date: 2004/03/10 10:42:11 $";

#include "pa_vxdoc.h"

// defines

#define SEARCH_NAMESPACES_NAME "search-namespaces"

Value* VXdoc::as(const char* atype, bool looking_up) {
	if(Value* result=Value::as(atype, looking_up))
		return result;
	else
		return VXnode::as(atype, looking_up);
}

/// VXdoc: $CLASS,$method
Value* VXdoc::get_element(const String& aname, Value& aself, bool looking_up) { 
	if(aname==SEARCH_NAMESPACES_NAME) {
		return &search_namespaces;
	}

	// up
	try {
		return VXnode::get_element(aname, aself, looking_up);
	} catch(Exception) { 
		// ignore bad node elements, they can be valid here...

		// fields
		GdomeDocument* document=get_document();
		GdomeException exc;

		if(aname=="doctype") {
			// readonly attribute DocumentType doctype;
			return new VXnode(fcharsets, *this, (GdomeNode*)gdome_doc_doctype(document, &exc));
		} else if(aname=="implementation") {
			// readonly attribute DOMImplementation implementation;
			return 0;
		} else if(aname=="documentElement") {
			// readonly attribute Element documentElement;
			return new VXnode(fcharsets, *this, (GdomeNode *)gdome_doc_documentElement(document, &exc));
		} 	

		return bark("%s field not found", &aname);
	}
}

#endif
