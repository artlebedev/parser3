/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dom.C,v 1.17 2001/09/11 09:20:57 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdom.h"
#include "pa_vfile.h"

#include <strstream>
#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>
#include <XMLSupport/FormatterToXML.hpp>
#include <XMLSupport/FormatterToHTML.hpp>
#include <XMLSupport/FormatterToText.hpp>
#include <XMLSupport/FormatterTreeWalker.hpp>

// defines

#define DOM_CLASS_NAME "dom"

#define DOM_OUTPUT_METHOD_OPTION_NAME "method"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"

#define DOM_OUTPUT_ENCODING_OPTION_NAME "encoding"

#define DOM_OUTPUT_DEFAULT_INDENT 4

// class

class MDom : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VDom(pool); }

public:
	MDom(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _set(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	Value& vxml=params->as_junction(0, "xml must be code");
	Temp_lang temp_lang(r, String::UL_XML);
	const String& xml=r.process(vxml).as_string();

	std::istrstream stream(xml.cstr());
	XalanParsedSource* parsedSource;
	int error=vDom.get_transformer().parseSource(&stream, parsedSource);

	if(error)
		PTHROW(0, 0,
			&method_name,
			vDom.get_transformer().getLastError());

	// replace any previous parsed source
	vDom.set_parsed_source(*parsedSource);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// filespec
	const String& filename=params->as_string(0, "file name must not be code");
	const char *filespec=r.absolute(filename).cstr(String::UL_FILE_NAME);
	
	XalanParsedSource* parsedSource;
	int error=vDom.get_transformer().parseSource(filespec, parsedSource);

	if(error)
		PTHROW(0, 0,
			&filename,
			vDom.get_transformer().getLastError());

	// replace any previous parsed source
	vDom.set_parsed_source(*parsedSource);
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


FormatterListener *create_optioned_listener(Pool& pool, 
								   const String& method_name, MethodParams *params, int index,
								   Writer& writer) {
	const String *method=0;
	XalanDOMString xalan_encoding;

	Value& voptions=params->as_no_junction(index, "options must not be code");
	if(voptions.is_defined()) {
		if(Hash *options=voptions.get_hash()) {
			// $.method[xml|html|text]
			if(Value *vmethod=static_cast<Value *>(options->get(*new(pool) 
				String(pool, DOM_OUTPUT_METHOD_OPTION_NAME))))
				method=&vmethod->as_string();

			// $.encoding[windows-1251|...]
			if(Value *vencoding=static_cast<Value *>(options->get(*new(pool) 
				String(pool, DOM_OUTPUT_ENCODING_OPTION_NAME)))) {
				const char *cstr=vencoding->as_string().cstr();
				xalan_encoding.append(cstr, strlen(cstr));
			}
		} else
			PTHROW(0, 0,
				&method_name,
				"options must be hash");
	}

	if(!method/*default='xml'*/ || *method == DOM_OUTPUT_METHOD_OPTION_VALUE_XML)
		return new FormatterToXML(writer,
			XalanDOMString(),  // version
			true, // doIndent
			DOM_OUTPUT_DEFAULT_INDENT, // indent 
			xalan_encoding  // encoding
		);
	else if(*method == DOM_OUTPUT_METHOD_OPTION_VALUE_HTML)
		return new FormatterToHTML(writer,
			xalan_encoding,  // encoding
			XalanDOMString(),  // mediaType 
			XalanDOMString(),  // doctypeSystem; String to be printed at the top of the document 
			XalanDOMString(),  // doctypePublic  
			true, // doIndent 
			DOM_OUTPUT_DEFAULT_INDENT // indent 
		);
	else if(*method == DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT)
		return new FormatterToText(writer,
			xalan_encoding  // encoding
		);
	else
		PTHROW(0, 0,
			method,
			DOM_OUTPUT_METHOD_OPTION_NAME " option is invalid; valid methods are: "
				"'" DOM_OUTPUT_METHOD_OPTION_VALUE_XML "', "
				"'" DOM_OUTPUT_METHOD_OPTION_VALUE_HTML "', "
				"'" DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT "'");			

	// never reached
	return 0; // calm, compiler
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// filespec
	const String& filename=params->as_string(1, "file name must not be code");
	const char *filespec=r.absolute(filename).cstr(String::UL_FILE_NAME);
	
	// document
	XalanDocument& document=vDom.get_document(pool, &method_name);

	try {
		XalanFileOutputStream stream(XalanDOMString(filespec, strlen(filespec)));
		XalanOutputStreamPrintWriter writer(stream);
		FormatterListener& formatterListener=*create_optioned_listener(pool, method_name, params, 0,
			writer);
		FormatterTreeWalker treeWalker(formatterListener);
		treeWalker.traverse(&document); // Walk the document and produce the XML...
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}

static void _string(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// document
	XalanDocument& document=vDom.get_document(pool, &method_name);

	try {
		String parserString=*new(pool) String(pool);
		ParserStringOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		FormatterListener& formatterListener=*create_optioned_listener(pool, method_name, params, 0,
			writer);
		FormatterTreeWalker treeWalker(formatterListener);
		treeWalker.traverse(&document); // Walk the document and produce the XML...

		// write out result
		r.write_no_lang(parserString);
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}


static void _file(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// document
	XalanDocument& document=vDom.get_document(pool, &method_name);

	try {
		String& parserString=*new(pool) String(pool);
		ParserStringOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		FormatterListener& formatterListener=*create_optioned_listener(pool, method_name, params, 0,
			writer);
		FormatterTreeWalker treeWalker(formatterListener);
		treeWalker.traverse(&document); // Walk the document and produce the XML...

		// write out result
		VFile& vfile=*new(pool) VFile(pool);
		const char *cstr=parserString.cstr();
		vfile.set(false/*tainted*/, cstr, strlen(cstr), 0/*filename*/, new(pool) String(pool, "text/xml"));
		r.write_no_lang(vfile);
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}


static void add_xslt_param(const Hash::Key& aattribute, Hash::Val *ameaning, 
								 void *info) {
	XalanTransformer& transformer=*static_cast<XalanTransformer *>(info);
	const char *attribute_cstr=aattribute.cstr();
	const char *meaning_cstr=static_cast<Value *>(ameaning)->as_string().cstr();

	transformer.setStylesheetParam(
		XalanDOMString(attribute_cstr, strlen(attribute_cstr)),  
		XalanDOMString(meaning_cstr, strlen(meaning_cstr)));
}
static void _xslt(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// params
	if(params->size()>1) {
		Value& vparams=params->as_no_junction(1, "params must not be code");
		if(vparams.is_defined())
			if(Hash *params=vparams.get_hash())
				params->for_each(add_xslt_param, &vDom.get_transformer());
			else
				PTHROW(0, 0,
					&method_name,
					"params must be hash");
	}

	// source
	XalanParsedSource &parsed_source=vDom.get_parsed_source(pool, &method_name);

	// stylesheet
	const String& stylesheet_filename=params->as_string(0, "file name must not be code");
	const char *stylesheet_filespec=r.absolute(stylesheet_filename).cstr(String::UL_FILE_NAME);

	// target
	XalanDocument* target=vDom.get_parser_liaison().createDocument();
	XSLTResultTarget domResultTarget(target);

	// transform
	int error=vDom.get_transformer().transform(parsed_source, stylesheet_filespec, domResultTarget);
	if(error)
		PTHROW(0, 0,
			&stylesheet_filename,
			vDom.get_transformer().getLastError());

	// write out result
	VDom& result=*new(pool) VDom(pool);
	result.set_document(*target);
	r.write_no_lang(result);
}

// constructor

MDom::MDom(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DOM_CLASS_NAME));

	// ^dom::set[<some>xml</some>]
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 1);

	// ^dom::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^dom.save[options hash;some.xml]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^dom.string[options hash] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 1, 1);

	// ^dom.file[options hash] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 1, 1);

	// ^dom.xslt[stylesheet filename]
	// ^dom.xslt[stylesheet filename;params hash]
	add_native_method("xslt", Method::CT_DYNAMIC, _xslt, 1, 2);

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
