/** @dom
	Parser: @b dom parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vxdoc.C,v 1.3 2001/10/19 14:15:23 parser Exp $
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vxdoc.C,v 1.3 2001/10/19 14:15:23 parser Exp $"; 

#include "pa_vxdoc.h"

#include <XalanDOM/XalanDocumentType.hpp>
#include <XalanDOM/XalanElement.hpp>

void VXdoc_cleanup(void *vxdoc) {
	//_asm int 3;
	static_cast<VXdoc *>(vxdoc)->cleanup();
}

/// VXdoc: $CLASS,$method
Value *VXdoc::get_element(const String& name) { 
	try {
		// $CLASS,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;

		// fields
		XalanDocument& document=get_document(pool(), &name);

		if(name=="doctype") {
			// readonly attribute DocumentType doctype;
			return NEW VXnode(pool(), document.getDoctype());
		} else if(name=="implementation") {
			// readonly attribute DOMImplementation implementation;
			return 0;
		} else if(name=="documentElement") {
			// readonly attribute Element documentElement;
			return NEW VXnode(pool(), document.getDocumentElement());
		} 	
	
	} catch(const XalanDOMException& e)	{
		Exception::convert(pool(), &name, e);
	}

	return 0;
}

#endif
