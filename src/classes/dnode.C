/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dnode.C,v 1.2 2001/09/18 16:05:42 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdnode.h"
#include "pa_vfile.h"

#include "dnode.h"

#include <util/XMLString.hpp>
//#include <XalanSourceTree/XalanSourceTreeInit.hpp>
#include <XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <XPath/XPathEvaluator.hpp>
#include <XPath/NodeRefList.hpp>
#include <XMLSupport/FormatterToXML.hpp>
#include <XMLSupport/FormatterToHTML.hpp>
#include <XMLSupport/FormatterToText.hpp>
#include <XMLSupport/FormatterTreeWalker.hpp>
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>

// defines

#define DNODE_CLASS_NAME "dnode"

#define DOM_OUTPUT_METHOD_OPTION_NAME "method"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define DOM_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"

#define DOM_OUTPUT_ENCODING_OPTION_NAME "encoding"

#define DOM_OUTPUT_DEFAULT_INDENT 4

// class

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


static void _xpath(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VDnode& vnode=*static_cast<VDnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must not be code");
	const char *expression_cstr=expression.cstr(String::UL_AS_IS);
	XalanDOMString dstring(expression_cstr);
	const XalanDOMChar *expression_dcstr=dstring.c_str();

	XPathEvaluator evaluator;
	// Initialize the XalanSourceTree subsystem...
//	XalanSourceTreeInit		theSourceTreeInit;
	// We'll use these to parse the XML file.
	XalanSourceTreeDOMSupport dom_support;

	try {
		NodeRefList list=evaluator.selectNodeList(dom_support, 
			&vnode.get_node(pool, &method_name), 
			expression_dcstr);

		VHash& result=*new(pool) VHash(pool);
		for(int i=0; i<list.getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash().put(skey, new(pool) VDnode(pool, list.item(i)));
		}
		result.set_name(method_name);
		r.write_no_lang(result);
	} catch(const XSLException& e) {
		_throw(pool, &expression, e);
	}
}

// constructor

MDnode::MDnode(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DNODE_CLASS_NAME));

	// ^node.save[options hash;some.xml]
	add_native_method("save", Method::CT_DYNAMIC, _save, 2, 2);

	// ^node.string[options hash] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 1, 1);

	// ^node.file[options hash] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 1, 1);

	// ^node.xpath[/some/xpath/query]
	add_native_method("xpath", Method::CT_DYNAMIC, _xpath, 1, 1);

}
// global variable

Methoded *Dnode_class;

// creator

Methoded *MDnode_create(Pool& pool) {
	return Dnode_class=new(pool) MDnode(pool);
}
