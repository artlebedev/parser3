/** @file
	Parser: @b xdoc parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_vxdoc.h,v 1.16 2002/01/14 17:48:57 paf Exp $
*/

#ifndef PA_VXDOC_H
#define PA_VXDOC_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vxnode.h"

//#include "libxml/xmlmemory.h"
//#include "libxml/parser.h"
//#include "libxslt/xslt.h"
//#include "libxslt/libxslt.h"
//#include "libxslt/extensions.h"
//#include "libexslt/exslt.h"
//#include "libxslt/xsltInternals.h"

extern Methoded *Xdoc_class;

void VXdoc_cleanup(void *);

/// value of type 'xdoc'. implemented with XalanDocument & co
class VXdoc : public VXnode {
	friend void VXdoc_destructor(void *);
public: // Value

	const char *type() const { return "xdoc"; }

	/// VXdoc: true
	bool as_bool() const { return true; }

	/// VXdoc: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VXnode: $CLASS,$method, fields
	Value *get_element(const String& name);

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // VDNode

	/// @test conv validity
	virtual GdomeNode *get_node(const String *source) { 
		return (GdomeNode *)get_document(source);
	}

public: // usage

	VXdoc(Pool& apool, GdomeDocument *adocument) : 
		VXnode(apool, 0, *Xdoc_class), 
		fdocument(adocument/*not adding ref, owning a doc*/) {
//		ftransformer(0) 

		register_cleanup(VXdoc_destructor, this);
//		ftransformer=new XalanTransformer2;
		memset(&output_options, 0, sizeof(output_options));
	}
protected:
	~VXdoc() {
		GdomeException exc;
		if(fdocument)			
			gdome_doc_unref(fdocument, &exc);

//		delete ftransformer;
	}
public:

	//XalanTransformer2& transformer() {return *ftransformer; }
	void set_document(GdomeDocument *adocument) { 
		GdomeException exc;
		if(fdocument)			
			gdome_doc_unref(fdocument, &exc);

		gdome_doc_ref(fdocument=adocument, &exc);
	}
	GdomeDocument *get_document(const String *source) { 
		if(!fdocument)
			throw Exception(0, 0,
				source,
				"can not be applied to uninitialized instance");
		return fdocument; 
	}

public:
	struct Output_options {
		const String *method;            /* the output method */
		const String *encoding;          /* encoding string */
		const String *mediaType;         /* media-type string */
		const String *doctypeSystem;     /* doctype-system string */
		const String *doctypePublic;     /* doctype-public string */
		bool indent;                 /* should output being indented */
		const String *version;           /* version string */
		bool standalone;             /* standalone = "yes" | "no" */
		bool omitXmlDeclaration;     /* omit-xml-declaration = "yes" | "no" */
	} output_options;
private:

	GdomeDocument *fdocument;
};

#endif
