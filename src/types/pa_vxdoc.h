/** @file
	Parser: @b xdoc parser class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXDOC_H
#define PA_VXDOC_H

static const char * const IDENT_VXDOC_H="$Date: 2005/08/05 13:03:06 $";

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

	/// VXdoc: true
	override Value& as_expr_result(bool /*return_string_as_is=false*/) { return *new VBool(as_bool()); }

	/// VXdoc: $CLASS,$method, fields
	override Value* get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // VXNode

	override GdomeNode* get_node() { 
		return (GdomeNode*)get_document();
	}

	override VXdoc& get_xdoc() {
		return *this;
	}

public: // usage

	VXdoc(Request_charsets* acharsets, GdomeDocument *adocument) : 
		VXnode(acharsets, *this, 0), 
		fdocument(0) {
		assign_document(adocument); // not adding ref, owning a doc
	}

	override ~VXdoc() {
		GdomeException exc;
		if(fdocument) {
			gdome_doc_unref(fdocument, &exc);
			assign_document(0);
		}
	}

public: // VXdoc

	void set_document(Request_charsets* acharsets, GdomeDocument *adocument) { 
		fcharsets=acharsets;

		GdomeException exc;
		if(fdocument)
			gdome_doc_unref(fdocument, &exc);

		assign_document(adocument);
		gdome_doc_ref(fdocument, &exc);
	}
	GdomeDocument* get_document() { 
		if(!fdocument)
			throw Exception("parser.runtime",
				0,
				"using unitialized xdoc object");
		return fdocument; 
	}

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

	/// hold reference to prevent premature collecting
	void assign_document(GdomeDocument *adocument) {
		fdocument=adocument;

		gcref_doc=fdocument?gdome_xml_doc_get_xmlDoc(fdocument):0;
	}

private:

	GdomeDocument* fdocument;
	xmlDoc *gcref_doc;
};

#endif
