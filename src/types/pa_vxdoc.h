/** @file
	Parser: @b xdoc parser class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXDOC_H
#define PA_VXDOC_H

#define IDENT_PA_VXDOC_H "$Id: pa_vxdoc.h,v 1.62 2020/12/15 17:10:44 moko Exp $"

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vxnode.h"
#include "pa_vhash.h"

// defines

#define VXDOC_TYPE "xdoc"

// externals

extern Methoded* xdoc_class;

struct XDocOutputOptions : public PA_Allocated {
	const String* method;       /* the output method */
	const String* encoding;     /* encoding string */
	const String* mediaType;    /* media-type string */
	int indent;                 /* should output being indented */
	const String* version;      /* version string */
	int standalone;             /* standalone = "yes" | "no" */
	int omitXmlDeclaration;     /* omit-xml-declaration = "yes" | "no" */
	const String* filename;     /* Parser3 option: filename */

	XDocOutputOptions() {
		memset(this, 0, sizeof(*this));
		indent=standalone=omitXmlDeclaration=-1;
	};

	void append(Request& r, HashStringValue* options, bool with_filename=false);
};

/// value of type 'xdoc'. implemented with libxml & co
class VXdoc: public VXnode {
public: // Value

	override const char* type() const { return VXDOC_TYPE; }
	override Value* as(const char* atype);

	override VStateless_class* get_class() { return xdoc_class; }

	/// VXdoc: true
	override bool as_bool() const { return true; }

	/// VXdoc: true	 
	override Value& as_expr_result();

	/// VFile: json-string
	override const String* get_json_string(Json_options& options);

	/// VXdoc: $method, fields
	override Value* get_element(const String& aname);

public: // VXNode

	override xmlNode& get_xmlnode() {
		return *reinterpret_cast<xmlNode*>(&get_xmldoc());
	}

	override VXdoc& get_vxdoc() {
		return *this;
	}

public: // usage

	VXdoc() : VXnode(*this), fcharsets(0), fdocument(0) {}

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
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized xdoc object");
		return *fdocument;
	}

	Request_charsets& charsets() {
		if(!fcharsets)
			throw Exception(PARSER_RUNTIME, 0, "using uninitialized xdoc object");
		return *fcharsets;
	}

	VXnode& wrap(xmlNode& anode);

public:

	VHash search_namespaces;

	XDocOutputOptions output_options;

private:

	Request_charsets* fcharsets;
	xmlDoc* fdocument;
};

#endif
