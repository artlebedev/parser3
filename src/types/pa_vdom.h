/** @file
	Parser: @b DOM parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdom.h,v 1.13 2001/09/25 09:36:51 parser Exp $
*/

#ifndef PA_VDOM_H
#define PA_VDOM_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vdnode.h"

#include <XercesParserLiaison/XercesParserLiaison.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
#include <XalanSourceTree/XalanSourceTreeParserLiaison.hpp>

extern Methoded *Dom_class;

void VDom_cleanup(void *);

/// value of type 'dom'. implemented with XalanDocument & co
class VDom : public VDnode {
	friend void VDom_cleanup(void *);
public: // Value

	const char *type() const { return "dom"; }
/*
	/// VDom: CLASS,method
	Value *get_element(const String& aname) {
		// VStateless_object $CLASS,$method
		if(Value *result=VStateless_object::get_element(aname))
			return result;
	}
*/
protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // VDNode

	XalanNode &get_node(Pool& pool, const String *source) { 
		return get_document(pool, source);
	}

public: // usage

	VDom(Pool& apool, XalanDocument *adocument=0) : VDnode(apool, 0, *Dom_class), 
		ftransformer(0),
		fparser_liaison(0),
		fparsed_source(0),
		fdocument(adocument) {
		register_cleanup(VDom_cleanup, this);
		ftransformer=new XalanTransformer;
		fparser_liaison=new XercesParserLiaison;
	}
private:
	void cleanup() {
		delete ftransformer;
		delete fparser_liaison;
	}
public:

	XalanTransformer& transformer() {return *ftransformer; }
	XercesParserLiaison& parser_liaison() { return *fparser_liaison; }

	void set_parsed_source(const XalanParsedSource& aparsed_source) { fparsed_source=&aparsed_source; }
	const XalanParsedSource& get_parsed_source(Pool& pool, const String *source) { 
		if(!fparsed_source)
			PTHROW(0, 0,
				source,
				"can not be performed on this instance (xslt[xslt] not supported, use save+xslt[load])");
		return *fparsed_source; 
	}

	void set_document(XalanDocument& adocument) { fdocument=&adocument; }
	XalanDocument &get_document(Pool& pool, const String *source) { 
		if(fparsed_source)
			return *fparsed_source->getDocument();
		if(!fdocument)
			PTHROW(0, 0,
				source,
				"can not be applied to uninitialized instance");
		return *fdocument; 
	}

private:

	XalanTransformer *ftransformer;
	XercesParserLiaison	*fparser_liaison;

	const XalanParsedSource *fparsed_source;
	XalanDocument *fdocument;

};

#endif
