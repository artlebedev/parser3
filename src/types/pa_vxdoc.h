/** @file
	Parser: @b DOM parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vxdoc.h,v 1.2 2001/10/05 16:12:40 parser Exp $
*/

#ifndef PA_VXDOC_H
#define PA_VXDOC_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"
#include "pa_vxnode.h"

#include <sax/HandlerBase.hpp>
#include <XercesParserLiaison/XercesParserLiaison.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
#include <XalanSourceTree/XalanSourceTreeParserLiaison.hpp>

extern Methoded *Xdoc_class;

void VXdoc_cleanup(void *);

/// value of type 'dom'. implemented with XalanDocument & co
class VXdoc : public VXnode {
	friend void VXdoc_cleanup(void *);
public: // Value

	const char *type() const { return "xdoc"; }
/*
	/// VXdoc: CLASS,method
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

	VXdoc(Pool& apool, XalanDocument *adocument=0) : VXnode(apool, 0, *Xdoc_class), 
		ftransformer(0),
		fparser_liaison(0), ferror_handler(0),
		fparsed_source(0),
		fdocument(adocument) {
		register_cleanup(VXdoc_cleanup, this);
		ftransformer=new XalanTransformer;
		fparser_liaison=new XercesParserLiaison;
		ferror_handler=new HandlerBase;
		fparser_liaison->setErrorHandler(ferror_handler); // disable stderr output
	}
private:
	void cleanup() {
		delete ftransformer;
		delete fparser_liaison;
		delete ferror_handler;
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
	ErrorHandler *ferror_handler;

	const XalanParsedSource *fparsed_source;
	XalanDocument *fdocument;

};

#endif
