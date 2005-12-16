/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

static const char * const IDENT_VXDOC="$Date: 2005/12/16 14:48:22 $";

#include "pa_vxdoc.h"

// defines

#define SEARCH_NAMESPACES_NAME "search-namespaces"


VXnode& VXdoc::wrap(xmlNode& anode) {
	VXnode* result;
	if((result=static_cast<VXnode*>(anode._private))) {
		assert(anode.doc==fdocument);
		return *result;
	}

	result=new VXnode(anode);
	anode._private=result;
	anode.doc=fdocument;

	return *result;
}


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
		xmlDoc& xmldoc=get_xmldoc();

		if(aname=="doctype") {
			// readonly attribute DocumentType doctype;
			if(xmlNode* node=(xmlNode*)xmldoc.intSubset)
				return &wrap(*node);
			else
				return 0;
		} else if(aname=="implementation") {
			// readonly attribute DOMImplementation implementation;
			return 0;
		} else if(aname=="documentElement") {
			// readonly attribute Element documentElement;
			xmlNode* rootElement=xmlDocGetRootElement(&xmldoc);
			assert(rootElement);
			return &wrap(*rootElement);
		} 	

		return bark("%s field not found", &aname);
	}
}

#endif
