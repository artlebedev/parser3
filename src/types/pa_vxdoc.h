/** @file
	Parser: @b xdoc parser class decl.

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXDOC_H
#define PA_VXDOC_H

static const char* IDENT_VXDOC_H="$Date: 2003/01/21 15:51:21 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vxnode.h"

// defines

#define VXDOC_TYPE "xdoc"

// externals

extern Methoded *Xdoc_class;

void VXdoc_cleanup(void *);

/// value of type 'xdoc'. implemented with libxml & co
class VXdoc : public VXnode {
	friend void VXdoc_destructor(void *);
public: // Value

	const char *type() const { return VXDOC_TYPE; }
	/*override*/ Value *as(const char *atype, bool looking_up);

	VStateless_class *get_class() { return Xdoc_class; }

	/// VXdoc: true
	bool as_bool() const { return true; }

	/// VXdoc: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VXnode: $CLASS,$method, fields
	Value *get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // VXNode

	virtual GdomeNode *get_node(const String *source) { 
		return (GdomeNode *)get_document(source);
	}

public: // usage

	VXdoc(Pool& apool, GdomeDocument *adocument) : 
		VXnode(apool, 0), 
		fdocument(adocument/*not adding ref, owning a doc*/) {

		register_cleanup(VXdoc_destructor, this);
		memset(&output_options, 0, sizeof(output_options));
	}
protected:
	~VXdoc() {
		GdomeException exc;
		if(fdocument)			
			gdome_doc_unref(fdocument, &exc);
	}
public:

	void set_document(GdomeDocument *adocument) { 
		GdomeException exc;
		if(fdocument)			
			gdome_doc_unref(fdocument, &exc);

		gdome_doc_ref(fdocument=adocument, &exc);
	}
	GdomeDocument *get_document(const String *source) { 
		if(!fdocument)
			throw Exception(0,
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
