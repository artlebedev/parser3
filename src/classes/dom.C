/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dom.C,v 1.3 2001/09/07 16:51:24 parser Exp $"; 

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

// defines

#define Dom_CLASS_NAME "dom"

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

// constructor

MDom::MDom(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), Dom_CLASS_NAME));

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


	return Dom_class=new(pool) MDom(pool);
}
