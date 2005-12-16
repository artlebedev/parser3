/** @file
	Parser: @b xnode parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXNODE_H
#define PA_VXNODE_H

static const char * const IDENT_VXNODE_H="$Date: 2005/12/16 10:15:12 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern "C" {
#include "libxml/tree.h"
};

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
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return *new VBool(as_bool()); }

	/// VXnode: $CLASS,$method, fields
	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

	/// VXnode: $nodeValue
	override const VJunction* put_element(Value& self, const String& aname, Value* avalue, bool replace);

public: // usage

	VXnode() : 
		fnode(0) {}

	VXnode(xmlNode& anode) : 
		fnode(&anode) {}

public: // VXnode

	virtual xmlNode& get_xmlnode() { 
		if(!fnode)
			throw Exception(0,
				0,
				"can not be applied to uninitialized instance");

		return *fnode; 
	}

	virtual VXdoc& get_vxdoc() {
		return get_internal_vxdoc();
	}

	Request_charsets& charsets();

private:

	VXdoc& get_internal_vxdoc() {
		throw Exception(0,0,"todo: VXdoc(fnode->doc->_private)");
		//return *(VXdoc*)0;
	}

private:

	xmlNode* fnode;
};

#endif
