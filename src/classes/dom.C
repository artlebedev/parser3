/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dom.C,v 1.9 2001/09/10 11:17:41 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdom.h"
#include "pa_vfile.h"

#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>
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

const char *strX(const XalanDOMString& s) {
	return XMLString::transcode(s.c_str());
}

static void _throw(Pool& pool, const String *source, const XSLException& e) {
	if(e.getURI().empty())
		PTHROW(0, 0,
			source,
			"%s (%s)",
				strX(e.getMessage()),  // message for exception
				strX(e.getType()) // type of exception
		);
	else
		PTHROW(0, 0,
			source,
			"%s (%s) %s(%d:%d)'", 
				strX(e.getMessage()),  // message for exception
				strX(e.getType()), // type of exception
				
				strX(e.getURI()),  // URI for the associated document, if any
				e.getLineNumber(),  // line number, or -1 if unknown
				e.getColumnNumber() // column number, or -1 if unknown
		);
}

class ParserStringOutputStream: public XalanOutputStream {
public:
	
	explicit ParserStringOutputStream(String& astring) : fstring(astring) {}

protected: // XalanOutputStream

	virtual void writeData(const char *theBuffer, unsigned long theBufferLength) {
		char *copy=(char *)fstring.malloc((size_t)theBufferLength);
		memcpy(copy, theBuffer, (size_t)theBufferLength);
		fstring.APPEND_CLEAN(copy, (size_t)theBufferLength, "dom", -1);
	}

	virtual void doFlush() {}

private:

	String& fstring;
	
};


static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDOM& vDOM=*static_cast<VDOM *>(r.self);

	// encoding
	const char *encoding=params->as_string(0, "encoding must not be code").cstr();

	// filespec
	const String& filename=params->as_string(1, "file name must not be code");
	const char *filespec=r.absolute(filename).cstr(String::UL_FILE_NAME);
	
	XalanParsedSource* parsedSource=vDOM.getParsedSource();
	if(!parsedSource)
		PTHROW(0, 0,
			&method_name,
			"on empty document");

	try {
		XalanDocument *document=parsedSource->getDocument();
		XalanFileOutputStream stream(XalanDOMString(filespec, strlen(filespec)));
		XalanOutputStreamPrintWriter writer(stream);
		FormatterToXML formatterListener(writer,
			XalanDOMString(),  // version
			true , // doIndent
			4, // indent 
			XalanDOMString(encoding, strlen(encoding))  // encoding
		);
		FormatterTreeWalker treeWalker(formatterListener);
		treeWalker.traverse(document); // Walk the document and produce the XML...
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}

static void _string(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDOM& vDOM=*static_cast<VDOM *>(r.self);

	// encoding
	const char *encoding=params->as_string(0, "encoding must not be code").cstr();

	XalanParsedSource* parsedSource=vDOM.getParsedSource();
	if(!parsedSource)
		PTHROW(0, 0,
			&method_name,
			"on empty document");

	try {
		XalanDocument *document=parsedSource->getDocument();
		String parserString=*new(pool) String(pool);
		ParserStringOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		FormatterToXML formatterListener(writer,
			XalanDOMString(),  // version
			true , // doIndent
			4, // indent 
			XalanDOMString(encoding, strlen(encoding)),  // encoding
			XalanDOMString(),  // mediaType
			XalanDOMString(),  // doctypeSystem
			XalanDOMString(),  // doctypePublic
			false // xmlDecl 
		);
		FormatterTreeWalker treeWalker(formatterListener);
		treeWalker.traverse(document); // Walk the document and produce the XML...

		// write out result
		r.write_no_lang(parserString);
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}


static void _file(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDOM& vDOM=*static_cast<VDOM *>(r.self);

	// encoding
	const char *encoding=params->as_string(0, "encoding must not be code").cstr();

	XalanParsedSource* parsedSource=vDOM.getParsedSource();
	if(!parsedSource)
		PTHROW(0, 0,
			&method_name,
			"on empty document");

	try {
		XalanDocument *document=parsedSource->getDocument();
		String& parserString=*new(pool) String(pool);
		ParserStringOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		FormatterToXML formatterListener(writer,
			XalanDOMString(),  // version
			true , // doIndent
			4, // indent 
			XalanDOMString(encoding, strlen(encoding))  // encoding
		);
		FormatterTreeWalker treeWalker(formatterListener);
		treeWalker.traverse(document); // Walk the document and produce the XML...

		// write out result
		VFile& vfile=*new(pool) VFile(pool);
		const char *cstr=parserString.cstr();
		vfile.set(false/*tainted*/, cstr, strlen(cstr), 0/*filename*/, new(pool) String(pool, "text/xml"));
		r.write_no_lang(vfile);
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}


// constructor

MDom::MDom(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DOM_CLASS_NAME));

	// ^dom::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^dom.save[encoding;some.xml]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^dom.string[encoding] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 1, 1);

	// ^dom.file[encoding] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 1, 1);

}
// global variable

Methoded *Dom_class;

// creator

Methoded *MDom_create(Pool& pool) {
	// Use the static initializers to initialize the Xalan-C++ and Xerces-C++ platforms. 
	// You must initialize Xerces-C++ once per process
	XMLPlatformUtils::Initialize();
	XalanTransformer::initialize();

	return Dom_class=new(pool) MDom(pool);
}
