/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dom.C,v 1.4 2001/09/10 08:23:49 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdom.h"

#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
//#include <DOMSupport/DOMServices.hpp> 
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <XMLSupport/FormatterToXML.hpp>
#include <XMLSupport/FormatterTreeWalker.hpp>

// defines

#define DOM_CLASS_NAME "dom"

// class

class MDom : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VDOM(pool, 0); }

public:
	MDom(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDOM& vDOM=*static_cast<VDOM *>(r.self);

	// filename
	const String& filename=params->as_string(0, "file name must not be code");

	// filespec
	const char *filespec=r.absolute(filename).cstr(String::UL_FILE_NAME);
	
//	XSLTInputSource::XSLTInputSource (  std::istream * stream )  
//	XalanNode *node=inputSource.getNode();
	XSLTInputSource inputSource(filespec);
	XalanParsedSource* parsedSource;
	int error=vDOM.getXalanTransformer().parseSource(inputSource, parsedSource);

	if(error)
		PTHROW(0, 0,
			&filename,
			vDOM.getXalanTransformer().getLastError());

	// replace any previous node value
	vDOM.setParsedSource(parsedSource);
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDOM& vDOM=*static_cast<VDOM *>(r.self);

	// filename
	const String& filename=params->as_string(0, "file name must not be code");

	// filespec
	const char *filespec=r.absolute(filename).cstr(String::UL_FILE_NAME);
	
	XalanParsedSource* parsedSource=vDOM.getParsedSource();
	if(!parsedSource)
		PTHROW(0, 0,
			&method_name,
			"on empty document");

	XalanDocument *document=parsedSource->getDocument();

/*
	XalanDOMString data;
	DOMServices::getNodeData(*document, data);
	const XalanDOMChar *cstr=data.c_str();
^^^^^^^^^^^just a text from one node
*/

	  //const XalanDOMString & theFileName
	XalanFileOutputStream fileOutputStream(XalanDOMString(filespec, strlen(filespec)));
	XalanOutputStreamPrintWriter outputStreamPrintWriter(fileOutputStream);
	FormatterToXML formatterListener(outputStreamPrintWriter);
/*
	// Sends the data for a node to a FormatterListener
	DOMServices::getNodeData(*document, formatterListener, 
		(DOMServices::MemberFunctionPtr)formatterListener.characters);
	formatterListener.endDocument();
*/
	// Create a FormatterTreeWalker with the the
	// new formatter...
	FormatterTreeWalker theTreeWalker(formatterListener);

	// Walk the document and produce the XML...
	theTreeWalker.traverse(document);
/*
	// Set up a XercesParserLiaison and use it to wrap the DOM_Document
	// in a XalanDocument.
	XercesDOMSupport   theDOMSupport;
	XercesParserLiaison	theParserLiaison(theDOMSupport);

	// You can also convert the XalanDocument to a Xerces DOM_Document.
	DOM_Document xercesDocument = theParserLiaison.mapXercesDocument(xalanDocument);
	//DOMPrint sample
*/
	// TODO error handling ...
}

// constructor

MDom::MDom(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DOM_CLASS_NAME));

	// ^dom::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^dom.save[some.xml]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 1);

}
// global variable

Methoded *Dom_class;

// creator

Methoded *MDom_create(Pool& pool) {
	// Use the static initializers to initialize the Xalan-C++ and Xerces-C++ platforms. 
	// You must initialize Xerces-C++ once per process
	XMLPlatformUtils::Initialize();
	XalanTransformer::initialize();
	// Must be called before any other functions are called. 
//	DOMServices::initialize (); 


	return Dom_class=new(pool) MDom(pool);
}
