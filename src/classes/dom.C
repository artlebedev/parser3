/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dom.C,v 1.26 2001/09/20 07:31:51 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdom.h"
#include "pa_xslt_stylesheet_manager.h"
#include "pa_stylesheet_connection.h"
#include "pa_vfile.h"
#include "dnode.h"

#include <strstream>
#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>
#include <XMLSupport/FormatterToXML.hpp>
#include <XMLSupport/FormatterToHTML.hpp>
#include <XMLSupport/FormatterToText.hpp>
#include <XMLSupport/FormatterTreeWalker.hpp>
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>

// defines

#define DOM_CLASS_NAME "dom"

#define DOM_OUTPUT_METHOD_OPTION_NAME "method"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"

#define DOM_OUTPUT_ENCODING_OPTION_NAME "encoding"

#define DOM_OUTPUT_DEFAULT_INDENT 4

// class

class MDom : public MDnode {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VDom(pool); }

public:
	MDom(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

// methods

class ParserStringXalanOutputStream: public XalanOutputStream {
public:
	
	explicit ParserStringXalanOutputStream(String& astring) : fstring(astring) {}

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

static void create_optioned_listener(
									 const char *& content_type, const char *& charset, FormatterListener *& listener, 
									 Pool& pool, 
									 const String& method_name, MethodParams *params, int index, Writer& writer) {
	charset=0;
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
				charset=vencoding->as_string().cstr();
				xalan_encoding.append(charset, strlen(charset));
			}
		} else
			PTHROW(0, 0,
				&method_name,
				"options must be hash");
	}

	if(!method/*default='xml'*/ || *method == DOM_OUTPUT_METHOD_OPTION_VALUE_XML) {
		content_type="text/xml";
		listener=new FormatterToXML(writer,
			XalanDOMString(),  // version
			true, // doIndent
			DOM_OUTPUT_DEFAULT_INDENT, // indent 
			xalan_encoding  // encoding
		);
	} else if(*method == DOM_OUTPUT_METHOD_OPTION_VALUE_HTML) {
		content_type="text/html";
		listener=new FormatterToHTML(writer,
			xalan_encoding,  // encoding
			XalanDOMString(),  // mediaType 
			XalanDOMString(),  // doctypeSystem; String to be printed at the top of the document 
			XalanDOMString(),  // doctypePublic  
			true, // doIndent 
			DOM_OUTPUT_DEFAULT_INDENT // indent 
		);
	} else if(*method == DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT) {
		content_type="text/plain";
		listener=new FormatterToText(writer,
			xalan_encoding  // encoding
		);
	} else
		PTHROW(0, 0,
			method,
			DOM_OUTPUT_METHOD_OPTION_NAME " option is invalid; valid methods are: "
				"'" DOM_OUTPUT_METHOD_OPTION_VALUE_XML "', "
				"'" DOM_OUTPUT_METHOD_OPTION_VALUE_HTML "', "
				"'" DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT "'");			

	// never reached
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDnode& vnode=*static_cast<VDnode *>(r.self);

	// filespec
	const String& file_name=params->as_string(1, "file name must not be code");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		XalanFileOutputStream stream(XalanDOMString(filespec, strlen(filespec)));
		XalanOutputStreamPrintWriter writer(stream);
		const char *content_type, *charset;
		FormatterListener *formatterListener;
		create_optioned_listener(content_type, charset, formatterListener, 
			pool, method_name, params, 0, writer);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}

static void _string(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDnode& vnode=*static_cast<VDnode *>(r.self);

	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		String parserString=*new(pool) String(pool);
		ParserStringXalanOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		const char *content_type, *charset;
		FormatterListener *formatterListener;
		create_optioned_listener(content_type, charset, formatterListener, 
			pool, method_name, params, 0, writer);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...

		// write out result
		r.write_no_lang(parserString);
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}


static void _file(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDnode& vnode=*static_cast<VDnode *>(r.self);

	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		String& parserString=*new(pool) String(pool);
		ParserStringXalanOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		const char *content_type, *charset;
		FormatterListener *formatterListener;
		create_optioned_listener(content_type, charset, formatterListener, 
			pool, method_name, params, 0, writer);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...

		// write out result
		VFile& vfile=*new(pool) VFile(pool);
		const char *cstr=parserString.cstr();
		String *scontent_type=new(pool) String(pool, content_type);
		Value *vcontent_type;
		if(charset) {
			VHash *vhcontent_type=new(pool) VHash(pool);
			vhcontent_type->hash().put(*value_name, new(pool) VString(*scontent_type));
			String *scharset=new(pool) String(pool, charset);
			vhcontent_type->hash().put(*new(pool) String(pool, "charset"), new(pool) VString(*scharset));
			vcontent_type=vhcontent_type;
		} else
			vcontent_type=new(pool) VString(*scontent_type);
		vfile.set(false/*tainted*/, cstr, strlen(cstr), 0/*file_name*/, vcontent_type);
		r.write_no_lang(vfile);
	} catch(const XSLException& e) {
		_throw(pool, &method_name, e);
	}
}

static void _set(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vdom=*static_cast<VDom *>(r.self);

	Value& vxml=params->as_junction(0, "xml must be code");
	Temp_lang temp_lang(r, String::UL_XML);
	const String& xml=r.process(vxml).as_string();

	std::istrstream stream(xml.cstr());
	XalanParsedSource* parsedSource;
	int error=vdom.transformer().parseSource(&stream, parsedSource);

	if(error)
		PTHROW(0, 0,
			&method_name,
			vdom.transformer().getLastError());

	// replace any previous parsed source
	vdom.set_parsed_source(*parsedSource);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vdom=*static_cast<VDom *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "file name must not be code");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	XalanParsedSource* parsedSource;
	int error=vdom.transformer().parseSource(filespec, parsedSource);

	if(error)
		PTHROW(0, 0,
			&file_name,
			vdom.transformer().getLastError());

	// replace any previous parsed source
	vdom.set_parsed_source(*parsedSource);
}

static void add_xslt_param(const Hash::Key& aattribute, Hash::Val *ameaning, 
						   void *info) {
	XalanTransformer& transformer=*static_cast<XalanTransformer *>(info);
	const char *attribute_cstr=aattribute.cstr();
	const char *meaning_cstr=static_cast<Value *>(ameaning)->as_string().cstr();

	transformer.setStylesheetParam(
		XalanDOMString(attribute_cstr),  
		XalanDOMString(meaning_cstr));
}
static void _xslt(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vdom=*static_cast<VDom *>(r.self);

	// params
	if(params->size()>1) {
		Value& vparams=params->as_no_junction(1, "transform parameters parameter must not be code");
		if(vparams.is_defined())
			if(Hash *params=vparams.get_hash())
				params->for_each(add_xslt_param, &vdom.transformer());
			else
				PTHROW(0, 0,
					&method_name,
					"transform parameters parameter must be hash");
	}

	// source
	XalanParsedSource &parsed_source=vdom.get_parsed_source(pool, &method_name);

	// stylesheet
	const String& stylesheet_file_name=params->as_string(0, "file name must not be code");
	const String& stylesheet_filespec=r.absolute(stylesheet_file_name);
	//_asm int 3;
	Stylesheet_connection& connection=XSLT_stylesheet_manager->get_connection(stylesheet_filespec);

	// target
	XalanDocument* target=vdom.parser_liaison().createDocument();
	XSLTResultTarget domResultTarget(target);

	// transform
	int error=vdom.transformer().transform(
		parsed_source, 
		&connection.stylesheet(), 
		domResultTarget);
	connection.close();
	if(error)
		PTHROW(0, 0,
			&stylesheet_file_name,
			vdom.transformer().getLastError());

	// write out result
	VDom& result=*new(pool) VDom(pool);
	result.set_document(*target);
	r.write_no_lang(result);
}

// constructor

MDom::MDom(Pool& apool) : MDnode(apool) {
	set_name(*NEW String(pool(), DOM_CLASS_NAME));

	// ^dom.save[options hash;some.xml]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^dom.string[options hash] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 1, 1);

	// ^dom.file[options hash] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 1, 1);

	// ^dom::set[<some>xml</some>]
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 1);

	// ^dom::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^dom.xslt[stylesheet file_name]
	// ^dom.xslt[stylesheet file_name;params hash]
	add_native_method("xslt", Method::CT_DYNAMIC, _xslt, 1, 2);

}
// global variable

Methoded *Dom_class;

// creator

Methoded *MDom_create(Pool& pool) {
	return Dom_class=new(pool) MDom(pool);
}
