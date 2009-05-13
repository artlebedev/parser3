/** @file
	Parser: @b xdoc parser class.

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

static const char * const IDENT_XDOC_C="$Date: 2009/05/13 07:36:05 $";

#include "libxml/tree.h"
#include "libxml/HTMLtree.h"
#include "libxslt/xsltInternals.h"
#include "libxslt/transform.h"
#include "libxslt/xsltutils.h"
#include "libxslt/variables.h"
#include "libxslt/imports.h"

#include "pa_vmethod_frame.h"

#include "pa_stylesheet_manager.h"
#include "pa_request.h"
#include "pa_vxdoc.h"
#include "pa_charset.h"
#include "pa_vfile.h"
#include "pa_xml_exception.h"
#include "xnode.h"

// defines

#define XDOC_CLASS_NAME "xdoc"

#define XDOC_OUTPUT_METHOD_OPTION_NAME "method"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"

// class

class MXdoc: public MXnode {
public: // VStateless_class
	Value* create_new_value(Pool&, HashStringValue&) { return new VXdoc(); }

public:
	MXdoc();

};

// global variable

DECLARE_CLASS_VAR(xdoc, new MXdoc, 0);

// helper classes

class xmlOutputBuffer_auto_ptr {
public:
	explicit xmlOutputBuffer_auto_ptr(xmlOutputBuffer *_APtr = 0) 
		: _Owns(_APtr != 0), _Ptr(_APtr) {}
	xmlOutputBuffer_auto_ptr(const xmlOutputBuffer_auto_ptr& _Y) 
		: _Owns(_Y._Owns), _Ptr(_Y.release()) {}
	xmlOutputBuffer_auto_ptr& operator=(const xmlOutputBuffer_auto_ptr& _Y) 
		{if (this != &_Y)
			{if (_Ptr != _Y.get())
				{if (_Owns && _Ptr)
					xmlOutputBufferClose(_Ptr);
				_Owns = _Y._Owns; }
			else if (_Y._Owns)
				_Owns = true;
			_Ptr = _Y.release(); }
		return (*this); }
	~xmlOutputBuffer_auto_ptr()
		{if (_Owns && _Ptr)
			xmlOutputBufferClose(_Ptr); }
	xmlOutputBuffer& operator*() const 
		{return (*get()); }
	xmlOutputBuffer *operator->() const 
		{return (get()); }
	xmlOutputBuffer *get() const 
		{return (_Ptr); }
	xmlOutputBuffer *release() const 
		{((xmlOutputBuffer_auto_ptr *)this)->_Owns = false;
		return (_Ptr); }
private:
	bool _Owns;
	xmlOutputBuffer *_Ptr;
};

class xsltTransformContext_auto_ptr {
public:
	explicit xsltTransformContext_auto_ptr(xsltTransformContext *_APtr = 0) 
		: _Owns(_APtr != 0), _Ptr(_APtr) {}
	xsltTransformContext_auto_ptr(const xsltTransformContext_auto_ptr& _Y) 
		: _Owns(_Y._Owns), _Ptr(_Y.release()) {}
	xsltTransformContext_auto_ptr& operator=(const xsltTransformContext_auto_ptr& _Y) 
		{if (this != &_Y)
			{if (_Ptr != _Y.get())
				{if (_Owns && _Ptr)
					xsltFreeTransformContext(_Ptr);
				_Owns = _Y._Owns; }
			else if (_Y._Owns)
				_Owns = true;
			_Ptr = _Y.release(); }
		return (*this); }
	~xsltTransformContext_auto_ptr()
		{if (_Owns && _Ptr)
			xsltFreeTransformContext(_Ptr); }
	xsltTransformContext& operator*() const 
		{return (*get()); }
	xsltTransformContext *operator->() const 
		{return (get()); }
	xsltTransformContext *get() const 
		{return (_Ptr); }
	xsltTransformContext *release() const 
		{((xsltTransformContext_auto_ptr *)this)->_Owns = false;
		return (_Ptr); }
private:
	bool _Owns;
	xsltTransformContext *_Ptr;
};

class xsltStylesheet_auto_ptr {
public:
	explicit xsltStylesheet_auto_ptr(xsltStylesheet *_APtr = 0) 
		: _Owns(_APtr != 0), _Ptr(_APtr) {}
	xsltStylesheet_auto_ptr(const xsltStylesheet_auto_ptr& _Y) 
		: _Owns(_Y._Owns), _Ptr(_Y.release()) {}
	xsltStylesheet_auto_ptr& operator=(const xsltStylesheet_auto_ptr& _Y) 
		{if (this != &_Y)
			{if (_Ptr != _Y.get())
				{if (_Owns && _Ptr)
					xsltFreeStylesheet(_Ptr);
				_Owns = _Y._Owns; }
			else if (_Y._Owns)
				_Owns = true;
			_Ptr = _Y.release(); }
		return (*this); }
	~xsltStylesheet_auto_ptr()
		{if (_Owns && _Ptr)
			xsltFreeStylesheet(_Ptr); }
	xsltStylesheet& operator*() const 
		{return (*get()); }
	xsltStylesheet *operator->() const 
		{return (get()); }
	xsltStylesheet *get() const 
		{return (_Ptr); }
	xsltStylesheet *release() const 
		{((xsltStylesheet_auto_ptr *)this)->_Owns = false;
		return (_Ptr); }
private:
	bool _Owns;
	xsltStylesheet *_Ptr;
};

// methods

static void writeNode(Request& r, VXdoc& xdoc, xmlNode* node) {
	if(!node)
		throw Exception(PARSER_RUNTIME,
			0,
			"error creating node"); // OOM, bad name, things like that

	// write out result
	r.write_no_lang(xdoc.wrap(*node));
}

struct IdsIteratorInfo {
	xmlChar *elementId;
	xmlNode *element;
};

/* Hash Scanner function for pa_getElementById */
extern "C" void // switching to calling convetion of libxml
idsHashScanner (void *payload, void *data, xmlChar *name) {
	IdsIteratorInfo *priv = (IdsIteratorInfo *)data;

	if (priv->element == NULL && xmlStrEqual (name, priv->elementId))
	{
		xmlNode* parent=((xmlID *)payload)->attr->parent;
		assert(parent);
		priv->element=parent;
	}
}

static xmlNode*
pa_getElementById(xmlDoc& xmldoc, xmlChar* elementId) {
	xmlHashTable *ids = (xmlHashTable *)xmldoc.ids;
	IdsIteratorInfo iter={elementId, NULL};
	xmlHashScan(ids, idsHashScanner, &iter);
	return iter.element;
}

/*
static xmlNode *
pa_importNode (xmlDoc& xmldoc, xmlNode& importedNode, bool deep) {
	xmlNode *result = NULL;

	switch (importedNode.type) {
	case XML_ATTRIBUTE_NODE:
		result = (xmlNode *)xmlCopyProp(xmldoc, (xmlAttr *)importedNode);
		result.parent=0; // no idea
		break;
	case XML_DOCUMENT_FRAG_NODE:
	case XML_ELEMENT_NODE:
	case XML_ENTITY_REF_NODE:
	case XML_PI_NODE:
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_COMMENT_NODE:
		result = xmlCopyNode (importedNode->n, deep);
		xmlSetTreeDoc (result, priv->n);
		break;
	default:
		*exc = GDOME_NOT_SUPPORTED_ERR;
	}

	return result;
}
*/

// Element createElement(in DOMString tagName) raises(DOMException);
static void _createElement(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* tagName=as_xmlchar(r, params, 0, "tagName must be string");

	xmlNode *node=xmlNewDocNode(&xmldoc, NULL, tagName, NULL);
	writeNode(r, vdoc, node);
}

// Element createElementNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
static void _createElementNS(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	// namespaceURI;localName
	xmlChar* namespaceURI=as_xmlchar(r, params, 0, NAMESPACEURI_MUST_BE_STRING);
	xmlChar* qualifiedName=as_xmlchar(r, params, 1, "qualifiedName must be string");

	xmlChar* prefix=0;
	xmlChar* localName=xmlSplitQName2(qualifiedName, &prefix);

	xmlNode *node;
	if(localName) {
		xmlNs& ns=pa_xmlMapNs(xmldoc, namespaceURI, prefix);
		node=xmlNewDocNode(&xmldoc, &ns, localName, NULL);
	} else
		node=xmlNewDocNode(&xmldoc, NULL, qualifiedName/*unqualified, actually*/, NULL);
	writeNode(r, vdoc, node);
}

// DocumentFragment createDocumentFragment()
static void _createDocumentFragment(Request& r, MethodParams&) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlNode *node=xmlNewDocFragment(&xmldoc);
	writeNode(r, vdoc, node);
}

// Text createTextNode(in DOMString data);
static void _createTextNode(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* data=as_xmlchar(r, params, 0, DATA_MUST_BE_STRING);

	xmlNode *node=xmlNewDocText(&xmldoc, data);
	writeNode(r, vdoc, node);
}

// Comment createComment(in DOMString data)
static void _createComment(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	xmlChar* data=as_xmlchar(r, params, 0, DATA_MUST_BE_STRING);

	xmlNode *node=xmlNewComment(data);
	writeNode(r, vdoc, node);
}

// CDATASection createCDATASection(in DOMString data) raises(DOMException);
static void _createCDATASection(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* data=as_xmlchar(r, params, 0, DATA_MUST_BE_STRING);

	xmlNode *node=xmlNewCDataBlock(&xmldoc, data, strlen((const char*)data));
	writeNode(r, vdoc, node);
}

// ProcessingInstruction createProcessingInstruction(in DOMString target,in DOMString data) raises(DOMException);
static void _createProcessingInstruction(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* target=as_xmlchar(r, params, 0, DATA_MUST_BE_STRING);
	xmlChar* data=as_xmlchar(r, params, 1, DATA_MUST_BE_STRING);

	xmlNode *node=xmlNewDocPI(&xmldoc, target, data);
	writeNode(r, vdoc, node);
}

// Attr createAttribute(in DOMString name) raises(DOMException);
static void _createAttribute(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* name=as_xmlchar(r, params, 0, NAME_MUST_BE_STRING);

	xmlNode *node=(xmlNode*)xmlNewDocProp(&xmldoc, name, 0);
	writeNode(r, vdoc, node);
}

// Attr createAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
static void _createAttributeNS(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* namespaceURI=as_xmlchar(r, params, 0, NAMESPACEURI_MUST_BE_STRING);
	xmlChar* qualifiedName=as_xmlchar(r, params, 1, NAME_MUST_BE_STRING);

	xmlChar* prefix=0;
	xmlChar* localName=xmlSplitQName2(qualifiedName, &prefix);

	xmlNode *node;
	if(localName) {
		xmlNs& ns=pa_xmlMapNs(xmldoc, namespaceURI, prefix);
		node=(xmlNode*)xmlNewDocProp(&xmldoc, localName, NULL);
		xmlSetNs(node, &ns);
	} else
		node=(xmlNode*)xmlNewDocProp(&xmldoc, qualifiedName/*unqualified, actually*/, NULL);
	writeNode(r, vdoc, node);
}

// EntityReference createEntityReference(in DOMString name) raises(DOMException);
static void _createEntityReference(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlChar* name=as_xmlchar(r, params, 0, NAME_MUST_BE_STRING);

	xmlNode *node=xmlNewReference(&xmldoc, name);
	writeNode(r, vdoc, node);
}


static void _getElementById(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	// elementId
	xmlChar* elementId=as_xmlchar(r, params, 0, "elementID must be string");

	if(xmlNode *node=pa_getElementById(xmldoc, elementId)) {
		// write out result
		writeNode(r, vdoc, node);
	}
}

static void _importNode(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xmlNode& importedNode=
		as_node(params, 0, "importedNode must be node");
	bool deep=
		params.as_bool(1, "deep must be bool", r);

	xmlNode *node=xmlDocCopyNode(&importedNode, &xmldoc, deep?1: 0);
	// write out result
	writeNode(r, vdoc, node);
}
/*
GdomeElement *gdome_doc_createElementNS (GdomeDocument *self, GdomeDOMString *namespaceURI, GdomeDOMString *qualifiedName, GdomeException *exc);
GdomeAttr *gdome_doc_createAttributeNS (GdomeDocument *self, GdomeDOMString *namespaceURI, GdomeDOMString *qualifiedName, GdomeException *exc);
*/

static void _create(Request& r, MethodParams& params) {
	Charset& source_charset=r.charsets.source();
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	Value& param=params[params.count()-1];
	xmlDoc* xmldoc;
	bool set_encoding=false;
	if(param.get_junction()) { // {<?xml?>...}
		Temp_lang temp_lang(r, String::L_XML);
		const String& xml=r.process_to_string(param);

		const char* cstr=xml.cstr(String::L_UNSPECIFIED, 0, &r.charsets);
		xmldoc=xmlParseMemory(cstr, strlen(cstr));
		//printf("document=0x%p\n", document);
		if(!xmldoc || xmlHaveGenericErrors())
			throw XmlException(0);

		// must be last action in if, see after if}
	} else { // [localName]
		if(const String* value = param.get_string()){
			xmlChar* localName=r.transcode(*value);
#if 0
			GdomeDocumentType *documentType=gdome_di_createDocumentType (
				docimpl, 
				r.transcode(qualifiedName), 
				0/*publicId*/, 
				0/*systemId*/, 
				&exc);
			if(!documentType || exc || xmlHaveGenericErrors())
				throw Exception(
					method_name, 
					exc);
			/// +xalan createXMLDecl ?
#endif
			xmldoc=xmlNewDoc(0);
			if(!xmldoc || xmlHaveGenericErrors())
				throw XmlException(0);
			xmlNode* node=xmlNewChild((xmlNode*)xmldoc, NULL, localName, NULL);
			if(!node || xmlHaveGenericErrors())
				throw XmlException(0);

			set_encoding=true;
			// must be last action in if, see after if}
		} else {
			VFile* vfile=param.as_vfile(String::L_UNSPECIFIED);
			xmldoc=xmlParseMemory(vfile->value_ptr(), vfile->value_size());
			if(!xmldoc || xmlHaveGenericErrors())
				throw XmlException(0);
		}
	}
	// must be first action after if}
	// replace any previous parsed source
	vdoc.set_xmldoc(r.charsets, *xmldoc); 
	
	// URI 
	const char* URI_cstr;
	if(params.count()>1) { // absolute(param)
		const String& URI=params.as_string(0, "URI must be string");
		URI_cstr=r.absolute(URI).cstr();
	} else // default = disk path to requested document
		URI_cstr=r.request_info.path_translated;
	if(URI_cstr)
		xmldoc->URL=source_charset.transcode_buf2xchar(URI_cstr, strlen(URI_cstr));

	if(set_encoding) {
		const char* source_charset_name=source_charset.NAME().cstr();
		xmldoc->encoding=source_charset.transcode_buf2xchar(source_charset_name, strlen(source_charset_name));
	}
}

static void _load(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// filespec
	const String* uri=&params.as_string(0, "URI must be string");
	const char* uri_cstr;
	if(uri->pos("://")==STRING_NOT_FOUND) // disk path
		uri_cstr=r.absolute(*uri).cstr(String::L_FILE_SPEC);
	else // xxx:// 
		uri_cstr=uri->cstr(String::L_AS_IS); // leave as-is for xmlParseFile to handle

	/// @todo!! add SAFE MODE!!
	xmlDoc* xmldoc=xmlParseFile(uri_cstr);
	if(!xmldoc || xmlHaveGenericErrors())
		throw XmlException(uri);
	
	// must be first action after if}
	// replace any previous parsed source
	vdoc.set_xmldoc(r.charsets, *xmldoc); 
}

static void param_option_over_output_option(
					    HashStringValue& param_options, const char* option_name,
					    const String*& output_option) {
	if(Value* value=param_options.get(String::Body(option_name)))
		output_option=&value->as_string();
}
static void param_option_over_output_option(
					    HashStringValue& param_options, const char* option_name,
					    int& output_option) {
	if(Value* value=param_options.get(String::Body(option_name))) {
		const String& s=value->as_string();
		if(s=="yes")
			output_option=1;
		else if(s=="no")
			output_option=0;
		else
			throw Exception(PARSER_RUNTIME,
				&s,
				"%s must be either 'yes' or 'no'", option_name);
	}
}

/// @test valid_options check
static void prepare_output_options(Request& r,
				   MethodParams& params, size_t index,
				   VXdoc::Output_options& oo) {
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

	// configuring with options from parameter...
	if(params.count()>index) {
		Value& voptions=params.as_no_junction(index, "options must be string");
		if(voptions.is_defined()) {
			if(HashStringValue *options=voptions.get_hash()) {
				// $.method[xml|html|text]
				if(Value* vmethod=options->get(String::Body(XDOC_OUTPUT_METHOD_OPTION_NAME)))
					oo.method=&vmethod->as_string();

				// $.version[1.0]
				param_option_over_output_option(*options, "version", oo.version);
				// $.encoding[windows-1251|...]
				param_option_over_output_option(*options, "encoding", oo.encoding);
				// $.omit-xml-declaration[yes|no]
				param_option_over_output_option(*options, "omit-xml-declaration", oo.omitXmlDeclaration);
				// $.standalone[yes|no]
				param_option_over_output_option(*options, "standalone", oo.standalone);
				// $.indent[yes|no]
				param_option_over_output_option(*options, "indent", oo.indent);
				// $.media-type[text/{html|xml|plain}]
				param_option_over_output_option(*options, "media-type", oo.mediaType);				 
			}
		}
	}

	// default encoding from pool
	if(!oo.encoding)
		oo.encoding=new String(r.charsets.source().NAME(), String::L_TAINTED);
	// default method=xml
	if(!oo.method)
		oo.method=new String(XDOC_OUTPUT_METHOD_OPTION_VALUE_XML);
	// default mediaType = depending on method
	if(!oo.mediaType) {
		if(*oo.method==XDOC_OUTPUT_METHOD_OPTION_VALUE_XML)
			oo.mediaType=new String("text/xml");
		else if(*oo.method==XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML)
			oo.mediaType=new String("text/html");
		else // XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT & all others
			oo.mediaType=new String("text/plain");
	}
}

struct Xdoc2buf_result {
	char* str;
	size_t length;
};
static Xdoc2buf_result xdoc2buf(Request& r, VXdoc& vdoc, 
					 MethodParams& params, int index,
					 VXdoc::Output_options& oo,
					 const String* file_spec,
					 bool use_source_charset_to_render_and_client_charset_to_write_to_header=false) {
	Xdoc2buf_result result;
	prepare_output_options(r, params, index,
		oo);

	const char* render_encoding;
	const char* header_encoding;
	if(use_source_charset_to_render_and_client_charset_to_write_to_header) {
		render_encoding=r.charsets.source().NAME_CSTR();
		header_encoding=r.charsets.client().NAME_CSTR();
	} else {
		header_encoding=render_encoding=oo.encoding->cstr();
	}

	xmlCharEncodingHandler *renderer=xmlFindCharEncodingHandler(render_encoding);
	if(!renderer)
		throw Exception(PARSER_RUNTIME,
			0,
			"encoding '%s' not supported", render_encoding);
	// UTF-8 renderer contains empty input/output converters, 
	// which is wrong for xmlOutputBufferCreateIO
	// while zero renderer goes perfectly 
	if(strcmp(render_encoding, "UTF-8")==0)
		renderer=0;

	xmlOutputBuffer_auto_ptr outputBuffer(xmlAllocOutputBuffer(renderer));

	xsltStylesheet_auto_ptr stylesheet(xsltNewStylesheet());
	if(!stylesheet.get())
		throw Exception(0,
			0,
			"xsltNewStylesheet failed");

	#define OOSTRING2STYLE(name) \
		stylesheet->name=oo.name?BAD_CAST xmlMemStrdup((const char*)r.transcode(*oo.name)):0
	#define OOBOOL2STYLE(name) \
		if(oo.name>=0) stylesheet->name=oo.name

	OOSTRING2STYLE(method);
	OOSTRING2STYLE(encoding);
	OOSTRING2STYLE(mediaType);
//	OOSTRING2STYLE(doctypeSystem);
//	OOSTRING2STYLE(doctypePublic);
	OOBOOL2STYLE(indent);
	OOSTRING2STYLE(version);
	OOBOOL2STYLE(standalone);
	OOBOOL2STYLE(omitXmlDeclaration);

	xmlDoc& xmldoc=vdoc.get_xmldoc();
	xmldoc.encoding=BAD_CAST xmlMemStrdup(render_encoding);
	if(header_encoding)
		stylesheet->encoding=BAD_CAST xmlMemStrdup(header_encoding);
	if(xsltSaveResultTo(outputBuffer.get(), &xmldoc, stylesheet.get())<0
		|| xmlHaveGenericErrors())
		throw XmlException(0);

	// write out result
	char *gnome_str;  size_t gnome_length;
	if(outputBuffer->conv) {
		gnome_length=outputBuffer->conv->use;
		gnome_str=(char *)outputBuffer->conv->content;
	} else {
		gnome_length=outputBuffer->buffer->use;
		gnome_str=(char *)outputBuffer->buffer->content;
	}

	if((result.length=gnome_length)) {
		result.str=pa_strdup(gnome_str, gnome_length);
	} else
		result.str=0;

	if(file_spec)
		file_write(*file_spec,
			gnome_str, gnome_length, 
			true/*as_text*/);

	return result;
}

static void _file(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	VXdoc::Output_options oo(vdoc.output_options);
	Xdoc2buf_result buf=xdoc2buf(r, vdoc, params, 0, 
		oo,
		0/*not to file, to memory*/);
	// write out result
	r.write_no_lang(String(buf.str));

	// write out result
	VFile& vfile=*new VFile;
	VHash& vhcontent_type=*new VHash;
	vhcontent_type.hash().put(
		value_name, 
		new VString(*oo.mediaType));
	vhcontent_type.hash().put(
		String::Body("charset"), 
		new VString(*oo.encoding));

	vfile.set(false/*tainted*/, buf.str?buf.str:""/*to distinguish from stat-ed file*/, buf.length, 
		0/*file_name*/, &vhcontent_type);
	r.write_no_lang(vfile);
}

static void _save(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& file_spec=r.absolute(params.as_string(0, FILE_NAME_MUST_BE_STRING));
	
	VXdoc::Output_options oo(vdoc.output_options);
	xdoc2buf(r, vdoc, params, 1, 
		oo,
		&file_spec);
}

static void _string(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	VXdoc::Output_options oo(vdoc.output_options);
	Xdoc2buf_result buf=xdoc2buf(r, vdoc, params, 0, 
		oo,
		0/*not to file, to memory*/,
		true/*use source charset to render, client charset to put to header*/);
	// write out result
	r.write_no_lang(String(String::Body(buf.str), String::L_AS_IS));
}

#ifndef DOXYGEN
struct Add_xslt_param_info {
	Request* r;
	Array<const xmlChar*>* strings;
	const xmlChar** current_transform_param;
};
#endif
static void add_xslt_param(
			   HashStringValue::key_type attribute, 
			   HashStringValue::value_type meaning, 
			   Add_xslt_param_info* info) {
	xmlChar* s;
	*info->current_transform_param++=(s=info->r->transcode(attribute)); *info->strings+=s;
	*info->current_transform_param++=(s=info->r->transcode(meaning->as_string())); *info->strings+=s;
}

static VXdoc& _transform(Request& r, const String* stylesheet_source, 
						   VXdoc& vdoc, xsltStylesheetPtr stylesheet, const xmlChar** transform_params) 
{
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	xsltTransformContext_auto_ptr transformContext(
		xsltNewTransformContext(stylesheet, &xmldoc));
	// make params literal
	if (transformContext->globalVars == NULL) // strangly not initialized by xsltNewTransformContext
		transformContext->globalVars = xmlHashCreate(20);
	xsltQuoteUserParams(transformContext.get(), (const char**)transform_params);
	// do transform
	xmlDoc *transformed=xsltApplyStylesheetUser(
		stylesheet,
		&xmldoc,
		0/*already quoted-inserted  transform_params*/,
		0/*const char* output*/,
		0/*FILE *profile*/,
		transformContext.get());
	if(!transformed || xmlHaveGenericErrors())
		throw XmlException(stylesheet_source);

	//gdome_xml_doc_mkref dislikes XML_HTML_DOCUMENT_NODE  type, fixing
	transformed->type=XML_DOCUMENT_NODE;
	// constructing result
	VXdoc& result=*new VXdoc(r.charsets, *transformed);
	/* grabbing options

		<xsl:output
		!method = "xml" | "html" | "text"
			X| qname-but-not-ncname 
		!version = nmtoken 
		!encoding = string 
		!omit-xml-declaration = "yes" | "no"
		!standalone = "yes" | "no"
		!doctype-public = string 
		!doctype-system = string 
		Xcdata-section-elements = qnames 
		!indent = "yes" | "no"
		!media-type = string /> 
	*/
	VXdoc::Output_options& oo=result.output_options;

	oo.method=stylesheet->method?&r.transcode(stylesheet->method):0;
	oo.encoding=stylesheet->encoding?&r.transcode(stylesheet->encoding):0;
	oo.mediaType=stylesheet->mediaType?&r.transcode(stylesheet->mediaType):0;
	oo.indent=stylesheet->indent;
	oo.version=stylesheet->version?&r.transcode(stylesheet->version):0;
	oo.standalone=stylesheet->standalone;
	oo.omitXmlDeclaration=stylesheet->omitXmlDeclaration;

	// return
	return result;
}
static void _transform(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// params
	Array<const xmlChar*> transform_strings;
	const xmlChar** transform_params=0;
	if(params.count()>1) {
		Value& vparams=params.as_no_junction(1, "transform parameters must be hash");
		if(!vparams.is_string())
			if(HashStringValue* hash=vparams.get_hash()) {
				transform_params=new(UseGC) const xmlChar*[hash->count()*2+1];
				Add_xslt_param_info info={
					&r, 
					&transform_strings,
					transform_params
				};
				hash->for_each<Add_xslt_param_info*>(add_xslt_param, &info);
				transform_params[hash->count()*2]=0;				
			} else
				throw Exception(PARSER_RUNTIME,
					0,
					"transform parameters parameter must be hash");
	}

	VXdoc* result;
	if(Value *vxdoc=params[0].as(VXDOC_TYPE, false)) { // stylesheet (xdoc)
		xmlDoc& stylesheetdoc=static_cast<VXdoc *>(vxdoc)->get_xmldoc();
		// compile xdoc stylesheet
		xsltStylesheet_auto_ptr stylesheet_ptr(xsltParseStylesheetDoc(&stylesheetdoc)); 
		if(xmlHaveGenericErrors())
			throw XmlException(0);
		if(!stylesheet_ptr.get())
			throw Exception("xml",
				0,
				"stylesheet failed to compile");
		// strange thing - xsltParseStylesheetDoc records document and destroys it in stylesheet destructor
		// we don't need that
		stylesheet_ptr->doc=0;

		// transform!
		result=&_transform(r, 0,
			vdoc, stylesheet_ptr.get(),
			transform_params);
	} else { // stylesheet (file name)
		// extablish stylesheet connection
		const String& stylesheet_filespec=
			r.absolute(params.as_string(0, "stylesheet must be file name (string) or DOM document (xdoc)"));
		Stylesheet_connection_ptr connection=stylesheet_manager->get_connection(stylesheet_filespec);

		// load and compile file to stylesheet [or get cached if any]
		// transform!
		result=&_transform(r, &stylesheet_filespec, vdoc, connection->stylesheet(),
			transform_params);
	}

	// write out result
	r.write_no_lang(*result);
}

// constructor

/// @test how to create empty type html?
MXdoc::MXdoc(): MXnode(XDOC_CLASS_NAME, xnode_class) {
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

	/// DOM2

	// ^xdoc.getElementById[elementId]
	add_native_method("getElementById", Method::CT_DYNAMIC, _getElementById, 1, 1);

    // Node (in Node importedNode, in boolean deep) raises(DOMException)
	add_native_method("importNode", Method::CT_DYNAMIC, _importNode, 2, 2);

	// Attr createAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
	add_native_method("createAttributeNS", Method::CT_DYNAMIC, _createAttributeNS, 2, 2);

	// Element createElementNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
	add_native_method("createElementNS", Method::CT_DYNAMIC, _createElementNS, 2, 2);

	/// parser
	
	// ^xdoc::create{qualifiedName}
	// ^xdoc::create[<some>xml</some>]
	// ^xdoc::create[URI][<some>xml</some>]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 2);	
	// for backward compatibility with <=v 1.82 2002/01/31 11:51:46 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 1);

	// ^xdoc::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^xdoc.save[some.xml]
	// ^xdoc.save[some.xml;options hash]
	add_native_method("save", Method::CT_DYNAMIC, _save, 1, 2);

	// ^xdoc.string[] <doc/>
	// ^xdoc.string[options hash] <doc/>
	add_native_method("string", Method::CT_DYNAMIC, _string, 0, 1);

	// ^xdoc.file[] file with "<doc/>"
	// ^xdoc.file[options hash] file with "<doc/>"
	add_native_method("file", Method::CT_DYNAMIC, _file, 0, 1);

	// ^xdoc.transform[stylesheet file_name/xdoc]
	// ^xdoc.transform[stylesheet file_name/xdoc;params hash]
	add_native_method("transform", Method::CT_DYNAMIC, _transform, 1, 2);

}

# else

#include "classes.h"

// global variable

DECLARE_CLASS_VAR(xdoc, 0, 0); // fictive

#endif
