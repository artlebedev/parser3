/** @file
	Parser: @b xdoc parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: xdoc.C,v 1.47 2001/11/22 09:59:38 paf Exp $
*/
#include "pa_types.h"
#include "classes.h"
#ifdef XML

#include "pa_request.h"
#include "pa_vxdoc.h"
#include "pa_stylesheet_manager.h"
#include "pa_stylesheet_connection.h"
#include "pa_vfile.h"
#include "xnode.h"

#include <strstream>
#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransENameMap.hpp>
#include "XalanTransformer2.hpp"
#include <XalanTransformer/XalanParsedSource.hpp>
#	include <XalanTransformer/XalanDefaultParsedSource.hpp>
#	include <XalanSourceTree/XalanSourceTreeDocument.hpp>
#	include <XalanSourceTree/XalanSourceTreeContentHandler.hpp>
#	include <sax2/XMLReaderFactory.hpp>
#include <XMLSupport/FormatterToXML.hpp>
#include <XMLSupport/FormatterToHTML.hpp>
#include <XMLSupport/FormatterToText.hpp>
#include <XMLSupport/FormatterTreeWalker.hpp>
#include <PlatformSupport/XalanFileOutputStream.hpp>
#include <PlatformSupport/XalanOutputStreamPrintWriter.hpp>
#include <PlatformSupport/DOMStringPrintWriter.hpp>
#include <XalanDOM/XalanElement.hpp>
#include <XalanDOM/XalanNodeList.hpp>
#include <XalanDOM/XalanDocumentFragment.hpp>
#include <XalanDOM/XalanCDATASection.hpp>
#include <XalanDOM/XalanEntityReference.hpp>
#include <dom/DOM_Document.hpp>
#include <XercesParserLiaison/XercesDocumentBridge.hpp>
#include <XalanTransformer/XercesDOMParsedSource.hpp>
#include <XSLT/StylesheetRoot.hpp>
#include <XalanTransformer/XalanCompiledStylesheet.hpp>

// defines

#define XDOC_CLASS_NAME "xdoc"

#define XDOC_OUTPUT_METHOD_OPTION_NAME "method"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"
#define XDOC_OUTPUT_DEFAULT_INDENT 4

// class

class MXdoc : public MXnode {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VXdoc(pool, 0, false); }

public:
	MXdoc(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
	void configure_admin(Request& r);
};

// methods

// Element createElement(in DOMString tagName) raises(DOMException);
static void _createElement(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& tagName=params->as_string(0, "tagName must be string");

	try {
		XalanNode *node=
			vdoc.get_document(pool, &method_name).
			createElement(*pool.transcode(tagName));
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node, false);
		r.write_no_lang(result);
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// DocumentFragment createDocumentFragment()
static void _createDocumentFragment(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	XalanNode *node=
		vdoc.get_document(pool, &method_name).
		createDocumentFragment();
	// write out result
	VXnode& result=*new(pool) VXnode(pool, node, false);
	r.write_no_lang(result);
}

// Text createTextNode(in DOMString data);
static void _createTextNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& data=params->as_string(0, "data must be string");

	XalanNode *node=
		vdoc.get_document(pool, &method_name).
		createTextNode(*pool.transcode(data));
	// write out result
	VXnode& result=*new(pool) VXnode(pool, node, false);
	r.write_no_lang(result);
}

// Comment createComment(in DOMString data)
static void _createComment(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& data=params->as_string(0, "data must be string");

	XalanNode *node=
		vdoc.get_document(pool, &method_name).
		createComment(*pool.transcode(data));
	// write out result
	VXnode& result=*new(pool) VXnode(pool, node, false);
	r.write_no_lang(result);
}

// CDATASection createCDATASection(in DOMString data) raises(DOMException);
static void _createCDATASection(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& data=params->as_string(0, "data must be string");

	try {
		XalanNode *node=
			vdoc.get_document(pool, &method_name).
			createCDATASection(*pool.transcode(data));
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node, false);
		r.write_no_lang(result);
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// ProcessingInstruction createProcessingInstruction(in DOMString target,in DOMString data) raises(DOMException);
static void _createProcessingInstruction(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& target=params->as_string(0, "data must be string");
	const String& data=params->as_string(1, "data must be string");

	try {
		XalanNode *node=
			vdoc.get_document(pool, &method_name).
			createProcessingInstruction(*pool.transcode(target), *pool.transcode(data));
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node, false);
		r.write_no_lang(result);
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// Attr createAttribute(in DOMString name) raises(DOMException);
static void _createAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& name=params->as_string(0, "name must be string");

	try {
		XalanNode *node=
			vdoc.get_document(pool, &method_name).
			createAttribute(*pool.transcode(name));
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node, false);
		r.write_no_lang(result);
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}
// EntityReference createEntityReference(in DOMString name) raises(DOMException);
static void _createEntityReference(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& name=params->as_string(0, "name must be string");

	try {
		XalanNode *node=
			vdoc.get_document(pool, &method_name).
			createEntityReference(*pool.transcode(name));
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node, false);
		r.write_no_lang(result);
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

/*
static void _getElementsByTagName(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// tagname
	const char *name=params->as_string(0, "name must be string").cstr(String::UL_XML);

	VHash& result=*new(pool) VHash(pool);
	if(const XalanNodeList *nodes=
		vdoc.get_document(pool, &method_name).getElementsByTagName(XalanDOMString(name))) {
		for(int i=0; i<nodes->getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash().put(skey, new(pool) VXnode(pool, nodes->item(i)));
		}
	}

	// write out result
	r.write_no_lang(result);
}

static void _getElementsByTagNameNS(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// namespaceURI;localName
	const char *namespaceURI=params->as_string(0, "namespaceURI must be string").cstr(String::UL_XML);
	const char *localName=params->as_string(0, "localName must be string").cstr(String::UL_XML);

	VHash& result=*new(pool) VHash(pool);
	if(const XalanNodeList *nodes=
		vdoc.get_document(pool, &method_name).getElementsByTagNameNS(
			XalanDOMString(namespaceURI), XalanDOMString(localName))) {
		for(int i=0; i<nodes->getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash().put(skey, new(pool) VXnode(pool, nodes->item(i)));
		}
	}

	// write out result
	r.write_no_lang(result);
}
*/


class ParserStringXalanOutputStream: public XalanOutputStream {
public:
	
	explicit ParserStringXalanOutputStream(String& astring) : fstring(astring) {}

protected: // XalanOutputStream

	virtual void writeData(const char *theBuffer, unsigned long theBufferLength) {
		char *copy=(char *)fstring.malloc((size_t)theBufferLength);
		memcpy(copy, theBuffer, (size_t)theBufferLength);
		fstring.APPEND_CLEAN(copy, (size_t)theBufferLength, "xdoc", 0);
	}

	virtual void doFlush() {}

private:

	String& fstring;
	
};

class XalanSourceTreeParserLiaison2: public XalanSourceTreeParserLiaison {
public:
	XalanSourceTreeParserLiaison2(XalanSourceTreeDOMSupport& 	theSupport) : XalanSourceTreeParserLiaison(theSupport), 
		ferror_handler(new HandlerBase) {
	}

	XalanDocument*
	parseXMLStream2(
				const InputSource& 	inputSource) {
		XalanSourceTreeContentHandler	theContentHandler(createXalanSourceTreeDocument());
		XalanAutoPtr<SAX2XMLReader>		theReader(XMLReaderFactory::createXMLReader());
		theReader->setContentHandler(&theContentHandler);
		theReader->setDTDHandler(&theContentHandler);
		theReader->setErrorHandler(ferror_handler); // disable stderr output
		theReader->setLexicalHandler(&theContentHandler);
		EntityResolver* const	theResolver = getEntityResolver();
		if (theResolver != 0) {
			theReader->setEntityResolver(theResolver);
		}
		theReader->parse(inputSource);
		return theContentHandler.getDocument();
	}
	
	~XalanSourceTreeParserLiaison2() {
		delete ferror_handler;
	}
private:
	ErrorHandler *ferror_handler;
};

class XalanDefaultParsedSource2 : public XalanParsedSource
{
public:

	XalanDefaultParsedSource2(const XSLTInputSource& 	theInputSource);

	virtual XalanDocument*
	getDocument() const;

	virtual XalanParsedSourceHelper*
	createHelper() const;

private:

	XalanSourceTreeDOMSupport		m_domSupport;

	XalanSourceTreeParserLiaison2	m_parserLiaison2;

	XalanSourceTreeDocument* const	m_parsedSource;
};

XalanDefaultParsedSource2::XalanDefaultParsedSource2(const XSLTInputSource& theInputSource):
	XalanParsedSource(),
	m_domSupport(),
	m_parserLiaison2(m_domSupport),
	m_parsedSource(m_parserLiaison2.mapDocument(m_parserLiaison2.parseXMLStream2(theInputSource)))
{
	assert(m_parsedSource != 0);

	m_domSupport.setParserLiaison(&m_parserLiaison2);
}



XalanDocument*	
XalanDefaultParsedSource2::getDocument() const
{
	return m_parsedSource;
}



XalanParsedSourceHelper*
XalanDefaultParsedSource2::createHelper() const
{
	return new XalanDefaultParsedSourceHelper(m_domSupport);
}


static void param_option_over_output_option(Pool& pool, 
											Hash *param_options, const char *option_name,
											XalanDOMString& output_option) {
	if(Value *value=static_cast<Value *>(param_options->get(*new(pool) 
		String(pool, option_name)))) {
		output_option.clear();
		output_option.append(value->as_string().cstr());
	}
}
static void param_option_over_output_option(Pool& pool, 
											Hash *param_options, const char *option_name,
											bool& output_option) {
	if(Value *value=static_cast<Value *>(param_options->get(*new(pool) 
		String(pool, option_name)))) {
		const String& s=value->as_string();
		if(s=="yes")
			output_option=true;
		else if(s=="no")
			output_option=false;
		else
			throw Exception(0, 0,
				&s,
				"%s must be either 'yes' or 'no'", option_name);
	}
}

static void create_optioned_listener(
									 Pool& pool, const String& method_name, MethodParams *params, int index,
									 VXdoc::Output_options& oo, Writer& writer,
									 FormatterListener *& listener) {
/*
	XalanDOMString encoding;
	XalanDOMString mediaType;
	XalanDOMString doctypeSystem;
	XalanDOMString doctypePublic;
	bool doIndent;
	XalanDOMString version;
	XalanDOMString standalone;
	bool xmlDecl;
*/

/*
<xsl:output
  !method = "xml" | "html" | "text" | qname-but-not-ncname 
  !version = nmtoken 
  !encoding = string 
  !omit-xml-declaration = "yes" | "no"
  !standalone = "yes" | "no"
  !doctype-public = string 
  !doctype-system = string 
  cdata-section-elements = qnames 
  !indent = "yes" | "no"
  !media-type = string /> 
*/

	/*
		fToXML->setStripCData(stripCData);
		fToXML->setEscapeCData(escapeCData);
	*/

	// configuring with options from parameter...
	if(params->size()>index) {
		Value& voptions=params->as_no_junction(index, "options must be string");
		if(voptions.is_defined()) {
			if(Hash *options=voptions.get_hash(&method_name)) {
				// $.method[xml|html|text]
				if(Value *vmethod=static_cast<Value *>(options->get(*new(pool) 
					String(pool, XDOC_OUTPUT_METHOD_OPTION_NAME))))
					oo.method=vmethod->as_string().cstr();

				// $.version[1.0]
				param_option_over_output_option(pool, options, "version", oo.version);
				// $.encoding[windows-1251|...]
				param_option_over_output_option(pool, options, "encoding", oo.encoding);
				// $.omit-xml-declaration[yes|no]
				bool omit_xml_declaration=!oo.xmlDecl;
				param_option_over_output_option(pool, options, "omit-xml-declaration", omit_xml_declaration);
				oo.xmlDecl=!omit_xml_declaration;
				// $.standalone[yes|no]
				param_option_over_output_option(pool, options, "standalone", oo.standalone);
				// $.doctype-public[?]
				param_option_over_output_option(pool, options, "doctype-public", oo.doctypePublic);
				// $.doctype-system[?]
				param_option_over_output_option(pool, options, "doctype-system", oo.doctypeSystem);
				// $.indent[yes|no]
				param_option_over_output_option(pool, options, "indent", oo.doIndent);
				// $.media-type[text/{html|xml|plain}]
				param_option_over_output_option(pool, options, "media-type", oo.mediaType);				 
			}
		}
	}

	// default encoding from pool
	if(oo.encoding.empty())
		oo.encoding.append(pool.get_charset().cstr());
	// default method=xml
	if(!oo.method)
		oo.method=XDOC_OUTPUT_METHOD_OPTION_VALUE_XML;

	if(strcmp(oo.method, XDOC_OUTPUT_METHOD_OPTION_VALUE_XML)==0) {
		if(oo.mediaType.empty())
			oo.mediaType.append("text/xml");
		listener=new FormatterToXML(writer,
			oo.version,  
			oo.doIndent,
			XDOC_OUTPUT_DEFAULT_INDENT, // indent 
			oo.encoding,
			oo.mediaType,
			oo.doctypeSystem,
			oo.doctypePublic,
			oo.xmlDecl,
			oo.standalone
		);
	} else if(strcmp(oo.method, XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML)==0) {
		if(oo.mediaType.empty())
			oo.mediaType.append("text/html");
		listener=new FormatterToHTML(writer,
			oo.encoding,
			oo.mediaType,
			oo.doctypeSystem,
			oo.doctypePublic,
			oo.doIndent,
			XDOC_OUTPUT_DEFAULT_INDENT, // indent 
			oo.version,
			oo.standalone,
			oo.xmlDecl
		);
	} else if(strcmp(oo.method, XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT)==0) {
		if(oo.mediaType.empty())
			oo.mediaType.append("text/plain");
		listener=new FormatterToText(writer,
			oo.encoding
		);
	} else
		throw Exception(0, 0,
			&method_name,
			XDOC_OUTPUT_METHOD_OPTION_NAME " option is invalid; valid methods are: "
				"'" XDOC_OUTPUT_METHOD_OPTION_VALUE_XML "', "
				"'" XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML "', "
				"'" XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT "'");			

	// never reached
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "file name must be string");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		VXdoc::Output_options oo(vdoc.output_options);
		XalanFileOutputStream stream(XalanDOMString(filespec, strlen(filespec)));
		XalanOutputStreamPrintWriter writer(stream);
		FormatterListener *formatterListener;
		create_optioned_listener(pool, method_name, params, 1, 
			oo, writer, formatterListener);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...
	} catch(const XSLException& e) {
		Exception::provide_source(pool, &method_name, e);
	}
}

static void _string(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// node
	XalanNode *node=&vdoc.get_document(pool, &method_name);//.getDocumentElement();
	if(!node)
		throw Exception(0, 0,
			&method_name,
			"no documentElement");

	try {
		VXdoc::Output_options oo(vdoc.output_options);
		String parserString=*new(pool) String(pool);
		ParserStringXalanOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		FormatterListener *formatterListener;
		create_optioned_listener(pool, method_name, params, 0, 
			oo, writer, formatterListener);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(node); // Walk that node and produce the XML...

		// write out result
		r.write_no_lang(parserString);
	} catch(const XSLException& e) {
		Exception::provide_source(pool, &method_name, e);
	}
}


static void _file(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// node
	XalanNode& node=vnode.get_node(pool, &method_name);

	try {
		VXdoc::Output_options oo(vdoc.output_options);
		String& parserString=*new(pool) String(pool);
		ParserStringXalanOutputStream stream(parserString);
		XalanOutputStreamPrintWriter writer(stream);
		FormatterListener *formatterListener;
		create_optioned_listener(pool, method_name, params, 0, 
			oo, writer, formatterListener);
		FormatterTreeWalker treeWalker(*formatterListener);
		treeWalker.traverse(&node); // Walk that node and produce the XML...

		// write out result
		VFile& vfile=*new(pool) VFile(pool);
		const char *cstr=parserString.cstr();
		const String& scontent_type=pool.transcode(oo.mediaType);
		Value *vcontent_type;
		if(oo.encoding.empty()) 
			vcontent_type=new(pool) VString(scontent_type);
		else {
			VHash *vhcontent_type=new(pool) VHash(pool);
			vhcontent_type->hash(&method_name).put(*value_name, new(pool) VString(scontent_type));
			const String& scharset=pool.transcode(oo.encoding);
			vhcontent_type->hash(&method_name).put(*new(pool) String(pool, "charset"), new(pool) VString(scharset));
			vcontent_type=vhcontent_type;
		}
		
		vfile.set(false/*tainted*/, cstr, strlen(cstr), 0/*file_name*/, vcontent_type);
		r.write_no_lang(vfile);
	} catch(const XSLException& e) {
		Exception::provide_source(pool, &method_name, e);
	}
}

static void _set(Request& r, const String& method_name, MethodParams *params) {
	//_asm int 3;
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	Value& vxml=params->as_junction(0, "xml must be code");
	Temp_lang temp_lang(r, String::UL_XML);
	const String& xml=r.process(vxml).as_string();

	std::istrstream stream(
		xml.cstr(String::UL_UNSPECIFIED, r.connection));
	const XalanParsedSource* parsedSource;

	try {
		parsedSource = new XalanDefaultParsedSource2(&stream);
	}
	catch (XSLException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
	catch (SAXParseException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
	catch (SAXException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
	catch (XMLException& e) {
		Exception::provide_source(pool, &method_name, e);
	}
	catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}

	// replace any previous parsed source
	vdoc.set_parsed_source(*parsedSource);
}

static void _create(Request& r, const String& method_name, MethodParams *params) {
	//_asm int 3;
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& qualifiedName=params->as_string(0, "qualifiedName must be string");

	XalanDocument& document=*new XercesDocumentBridge(
		DOM_Document::createDocument(),
		0,
		false /*threadSafe*/,
		false /*don' buildBridge -- too early, empty document*/);

	/// +createXMLDecl ?
	document.appendChild(document.createElement(*pool.transcode(qualifiedName)));

	// replace any previous document
	vdoc.set_document(document, true/*owns*/);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "file name must be string");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	const XalanParsedSource* parsedSource;
	try {
		parsedSource = new XalanDefaultParsedSource2(filespec);
	}
	catch (XSLException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
	catch (SAXParseException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
	catch (SAXException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
	catch (XMLException& e) {
		Exception::provide_source(pool, &method_name, e);
	}
	catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}

	// replace any previous parsed source
	vdoc.set_parsed_source(*parsedSource);
}

/// @test lang=String::UL_UNSPECIFIED?
static void add_xslt_param(const Hash::Key& aattribute, Hash::Val *ameaning, 
						   void *info) {
	XalanTransformer2& transformer=*static_cast<XalanTransformer2 *>(info);
	const char *attribute_cstr=aattribute.cstr(String::UL_UNSPECIFIED);
	const char *meaning_cstr=static_cast<Value *>(ameaning)->as_string().cstr(String::UL_UNSPECIFIED);

	transformer.setStylesheetParam(
		XalanDOMString(attribute_cstr),  
		XalanDOMString(meaning_cstr));
}
static void _transform(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// params
	if(params->size()>1) {
		Value& vparams=params->as_no_junction(1, "transform parameters parameter must be string");
		if(vparams.is_defined())
			if(Hash *params=vparams.get_hash(&method_name))
				params->for_each(add_xslt_param, &vdoc.transformer());
			else
				throw Exception(0, 0,
					&method_name,
					"transform parameters parameter must be hash");
	}

	// stylesheet
	const String& stylesheet_file_name=params->as_string(0, "file name must be string");
	const String& stylesheet_filespec=r.absolute(stylesheet_file_name);
	//_asm int 3;
	Stylesheet_connection& connection=stylesheet_manager->get_connection(stylesheet_filespec);

	// target
	XalanDocument* target=vdoc.parser_xerces_liaison().createDocument();

	// transform
	try {
		// note: 
		//   actually, never found any difference between the two
		//   but still there some extra "xerces" words at start of transform body
		//   wich were originally "xalan"
		//   not daring to change that

		const XalanCompiledStylesheet& stylesheet=connection.stylesheet(false/*nocache*/);
		if(vdoc.has_parsed_source()) { // set|load, not create?
			vdoc.transformer().transform2(
				vdoc.get_parsed_source(pool, &method_name), 
				&stylesheet, 
				target);
		} else {
			target=vdoc.parser_xerces_liaison().createDocument();
			vdoc.transformer().transform2(
				vdoc.get_document(pool, &method_name), 
				&stylesheet, 
				target);
		}
		VXdoc& result=*new(pool) VXdoc(pool, target, false/*owns not*/);
		// write out result
		r.write_no_lang(result);
		//
		if(const StylesheetRoot *stylesheetRoot=stylesheet.getStylesheetRoot()) {
			/*
			<xsl:output
			!method = "xml" | "html" | "text"
				X| qname-but-not-ncname 
			!version = nmtoken 
			!encoding = string 
			!omit-xml-declaration = "yes" | "no"
			!standalone = "yes" | "no"
			!doctype-public = string 
			!doctype-system = string 
			cdata-section-elements = qnames 
			!indent = "yes" | "no"
			!media-type = string /> 
			*/

			VXdoc::Output_options& oo=result.output_options;
			oo.encoding=stylesheetRoot->m_encoding;
			oo.mediaType=stylesheetRoot->m_mediatype;
			oo.doctypeSystem=stylesheetRoot->m_doctypeSystem;
			oo.doctypePublic=stylesheetRoot->m_doctypePublic;
			oo.doIndent=stylesheetRoot->m_indentResult;
			oo.version=stylesheetRoot->m_version;
			oo.standalone=stylesheetRoot->m_standalone;
			oo.xmlDecl=!stylesheetRoot->m_omitxmlDecl;

			switch(stylesheetRoot->getOutputMethod()) {
			case FormatterListener::OUTPUT_METHOD_HTML: 
				oo.method=XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML; break;
			case FormatterListener::OUTPUT_METHOD_TEXT: 
				oo.method=XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT; break;
			case FormatterListener::OUTPUT_METHOD_NONE: 
			case FormatterListener::OUTPUT_METHOD_XML: 
				oo.method=XDOC_OUTPUT_METHOD_OPTION_VALUE_XML; break;
			default:
				throw Exception(0, 0,
					&method_name,
					"unsupported output method specified"); 
				break; // never
			}
		}
	}
	catch (XSLException& e)	{
		connection.close();
		Exception::provide_source(pool, &stylesheet_file_name, e);
	}
	catch (SAXParseException& e)	{
		connection.close();
		Exception::provide_source(pool, &stylesheet_file_name, e);
	}
	catch (SAXException& e)	{
		connection.close();
		Exception::provide_source(pool, &stylesheet_file_name, e);
	}
	catch (XMLException& e) {
		connection.close();
		Exception::provide_source(pool, &stylesheet_file_name, e);
	}
	catch(const XalanDOMException& e)	{
		connection.close();
		Exception::provide_source(pool, &stylesheet_file_name, e);
	}

	// close
	connection.close();
}

static void _getElementById(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// elementId
	const String& elementId=params->as_string(0, "elementID must be string");

	if(XalanNode *node=
		vdoc.get_document(pool, &method_name).getElementById(*pool.transcode(elementId))) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node, false);
		r.write_no_lang(result);
	}
}
// constructor

MXdoc::MXdoc(Pool& apool) : MXnode(apool) {
	set_name(*NEW String(pool(), XDOC_CLASS_NAME));

	/// @test how to create empty type html?

	/// DOM1

	// Element createElement(in DOMString tagName) raises(DOMException);
	add_native_method("createElement", Method::CT_DYNAMIC, _createElement, 1, 1);
	// DocumentFragment createDocumentFragment(); 
	add_native_method("createDocumentFragment", Method::CT_DYNAMIC, _createDocumentFragment, 0, 0);
	// Text createTextNode(in DOMString data);
	add_native_method("createTextNode", Method::CT_DYNAMIC, _createTextNode, 1, 1);
	// Comment createComment(in DOMString data);
	add_native_method("createComment", Method::CT_DYNAMIC, _createComment, 1, 1);
	// CDATASection createCDATASection(in DOMString data) raises(DOMException);
	add_native_method("createCDATASection", Method::CT_DYNAMIC, _createCDATASection, 1, 1);
	// ProcessingInstruction createProcessingInstruction(in DOMString target, in DOMString data) raises(DOMException);
	add_native_method("createProcessingInstruction", Method::CT_DYNAMIC, _createProcessingInstruction, 2, 2);
	// Attr createAttribute(in DOMString name) raises(DOMException);
	add_native_method("createAttribute", Method::CT_DYNAMIC, _createAttribute, 1, 1);
	// EntityReference createEntityReference(in DOMString name) raises(DOMException);
	add_native_method("createEntityReference", Method::CT_DYNAMIC, _createEntityReference, 1, 1);
	// NodeList getElementsByTagName(in DOMString tagname);
	/*	
		// ^xdoc.getElementsByTagName[tagname]
		add_native_method("getElementsByTagName", Method::CT_DYNAMIC, _getElementsByTagName, 1, 1);

		// ^xdoc.getElementsByTagNameNS[namespaceURI;localName] = array of nodes
		add_native_method("getElementsByTagNameNS", Method::CT_DYNAMIC, _getElementsByTagNameNS, 2, 2);
	*/

	/// DOM2(?)

	// ^xdoc.getElementById[elementId]
	add_native_method("getElementById", Method::CT_DYNAMIC, _getElementById, 1, 1);

	/// parser
	
	// ^xdoc.save[some.xml]
	// ^xdoc.save[some.xml;options hash]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^xdoc.string[] <doc/>
	// ^xdoc.string[options hash] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 0, 1);

	// ^xdoc.file[] file with "<doc/>"
	// ^xdoc.file[options hash] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 0, 1);

	// ^xdoc::set[<some>xml</some>]
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 1);
	// ^xdoc::create{qualifiedName}
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 1);	

	// ^xdoc::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^xdoc.transform[stylesheet file_name]
	// ^xdoc.transform[stylesheet file_name;params hash]
	add_native_method("transform", Method::CT_DYNAMIC, _transform, 1, 2);

}

void MXdoc::configure_admin(Request& r) {
}

// global variable

Methoded *Xdoc_class;

// creator

#endif

Methoded *MXdoc_create(Pool& pool) {
	return 
#ifdef XML
		Xdoc_class=new(pool) MXdoc(pool)
#else
		0
#endif
	;
}
