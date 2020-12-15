/** @file
	Parser: @b xnode parser class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXNODE_H
#define PA_VXNODE_H

#define IDENT_PA_VXNODE_H "$Id: pa_vxnode.h,v 1.49 2020/12/15 17:10:44 moko Exp $"

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

#include "libxml/tree.h"

// defines

#define VXNODE_TYPE "xnode"

// externals

extern Methoded* xnode_class;

// forwards

class VXdoc;

/// value of type 'xnode'. implemented with xmlNode
class VXnode: public VStateless_object {
public: // Value

	override const char* type() const { return VXNODE_TYPE; }
	override VStateless_class* get_class() { return xnode_class; }

	/// VXnode: true
	override bool as_bool() const { return true; }

	/// VXnode: true	 
	override Value& as_expr_result();

	/// VXnode: $method, fields
	override Value* get_element(const String& aname);

	/// VXnode: $nodeValue
	override const VJunction* put_element(const String& aname, Value* avalue);

public: // usage

	VXnode(xmlNode& anode) : 
		fnode(anode) {}

public: // VXnode

	virtual xmlNode& get_xmlnode() { 
		return fnode; 
	}

	virtual VXdoc& get_vxdoc() {
		assert(fnode.doc);
		VXdoc* result=static_cast<VXdoc*>(fnode.doc->_private);
		assert(result);
		return *result;
	}

	Request_charsets& charsets();

private:

	xmlNode& fnode;
};

#endif
