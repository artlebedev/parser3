/** @file
	Parser: @b xnode parser class decl.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXNODE_H
#define PA_VXNODE_H

static const char * const IDENT_VXNODE_H="$Date: 2005/07/15 06:16:42 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

extern "C" {
#include "libxml/tree.h"

#include "gdome.h"
#include "gdomecore/gdome-xml-node.h"
#include "gdomecore/gdome-xml-document.h"
};

// defines

#define VXNODE_TYPE "xnode"

// helper defines

#define gdome_xml_doc_get_xmlDoc(dome_doc) (((_Gdome_xml_Document *)dome_doc)->n)

// externals

extern Methoded* xnode_class;

// forwards

class VXdoc;

/// value of type 'xnode'. implemented with GdomeNode
class VXnode: public VStateless_object, PA_Cleaned {
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
	override const Method* put_element(const String& aname, Value* avalue, bool /*replace*/);

public: // usage

	VXnode(Request_charsets* acharsets, VXdoc& adocument, GdomeNode* anode) : 
		fcharsets(acharsets),
		fdocument(adocument),
		fnode(anode/*not adding ref, owning a node*/) {
	}

	override ~VXnode() {
		GdomeException exc;
		if(fnode)			
			gdome_n_unref(fnode, &exc);
	}

public: // VXnode

	virtual GdomeNode* get_node() { 
		if(!fnode)
			throw Exception(0,
				0,
				"can not be applied to uninitialized instance");

		return fnode; 
	}

	virtual VXdoc& get_xdoc() {
		return fdocument;
	}

protected:

	Request_charsets* fcharsets;

private:

	VXdoc& fdocument;
	GdomeNode* fnode;
};

#endif
