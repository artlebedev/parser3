/** @file
	Parser: @b xdoc parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXDOC_H
#define PA_VXDOC_H

static const char * const IDENT_VXDOC_H="$Date: 2007/02/03 18:08:39 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vxnode.h"
#include "pa_vhash.h"

// defines

#define VXDOC_TYPE "xdoc"

// externals

extern Methoded* xdoc_class;

/// value of type 'xdoc'. implemented with libxml & co
class VXdoc: public VXnode {
public: // Value

	override const char* type() const { return VXDOC_TYPE; }
	override Value* as(const char* atype, bool looking_up);

	override VStateless_class* get_class() { return xdoc_class; }

	/// VXdoc: true
	override bool as_bool() const { return true; }

	/// VXdoc: $CLASS,$method, fields
	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // VXNode

	override xmlNode& get_xmlnode() { 
		return *reinterpret_cast<xmlNode*>(&get_xmldoc());
	}

	override VXdoc& get_vxdoc() {
		return *this;
	}

public: // usage

	VXdoc() : 
		VXnode(*this), 
		fcharsets(0),
		fdocument(0) {}

	VXdoc(Request_charsets& acharsets, xmlDoc& adocument) : VXnode(*this) {
		set_xmldoc(acharsets, adocument);
	}

public: // VXdoc

	void set_xmldoc(Request_charsets& acharsets, xmlDoc& adocument) { 
		fcharsets=&acharsets;
		fdocument=&adocument;
		fdocument->_private=this;
	}
	xmlDoc& get_xmldoc() { 
		if(!fdocument)
			throw Exception("parser.runtime",
				0,
				"using unitialized xdoc object");
		return *fdocument; 
	}

	Request_charsets& charsets() { 
		if(!fcharsets)
			throw Exception("parser.runtime",
				0,
				"using unitialized xdoc object");
		return *fcharsets; 
	}

	VXnode& wrap(xmlNode& anode);

public:

	VHash search_namespaces;

	struct Output_options {
		const String* method;            /* the output method */
		const String* encoding;          /* encoding string */
		const String* mediaType;         /* media-type string */
		int indent;                 /* should output being indented */
		const String* version;           /* version string */
		int standalone;             /* standalone = "yes" | "no" */
		int omitXmlDeclaration;     /* omit-xml-declaration = "yes" | "no" */

		Output_options() {
			memset(this, 0, sizeof(*this));
			indent=standalone=omitXmlDeclaration=-1;
		};
	} output_options;

private:

	Request_charsets* fcharsets;
	xmlDoc* fdocument;
};

#endif
