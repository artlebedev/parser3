/** @file
	Parser: @b xdoc parser class.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_config_includes.h"

#ifdef XML

static const char* IDENT_XDOC_C="$Date: 2003/09/25 09:15:02 $";

#include "gdome.h"
#include "libxml/tree.h"
#include "libxslt/xsltInternals.h"
#include "libxslt/transform.h"
#include "libxslt/xsltutils.h"
#include "libxslt/variables.h"


#include "pa_vmethod_frame.h"

#include "pa_stylesheet_manager.h"
#include "pa_request.h"
#include "pa_vxdoc.h"
#include "pa_charset.h"
#include "pa_vfile.h"
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
	Value* create_new_value() { return new VXdoc(0, 0); }

public:
	MXdoc();

public: // Methoded
	void configure_admin(Request& r);
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

static void writeNode(Request& r, GdomeNode *node, 
					  GdomeException exc) {
	if(!node || exc)
		throw Exception(0, exc);

	// write out result
	r.write_no_lang(*new VXnode(&r.charsets, node));
}

// Element createElement(in DOMString tagName) raises(DOMException);
static void _createElement(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& tagName=params.as_string(0, "tagName must be string");

	GdomeException exc;
	GdomeNode *node=
		(GdomeNode *)gdome_doc_createElement(vdoc.get_document(), 
		r.transcode(tagName).use(),
		&exc);
	writeNode(r, node, exc);
}

// Element createElementNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
static void _createElementNS(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// namespaceURI;localName
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& qualifiedName=params.as_string(1, "qualifiedName must be string");

	GdomeException exc;
	GdomeNode *node=
		(GdomeNode *)gdome_doc_createElementNS(vdoc.get_document(), 
		r.transcode(namespaceURI).use(),
		r.transcode(qualifiedName).use(),
		&exc);
	writeNode(r, node, exc);
}

// DocumentFragment createDocumentFragment()
static void _createDocumentFragment(Request& r, MethodParams&) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	GdomeException exc;
	GdomeNode *node=
		(GdomeNode *)gdome_doc_createDocumentFragment(
		vdoc.get_document(),
		&exc);
	writeNode(r, node, exc);
}

// Text createTextNode(in DOMString data);
static void _createTextNode(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& data=params.as_string(0, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createTextNode(
		vdoc.get_document(),
		r.transcode(data).use(),
		&exc);
	writeNode(r, node, exc);
}

// Comment createComment(in DOMString data)
static void _createComment(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& data=params.as_string(0, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createComment(
		vdoc.get_document(),
		r.transcode(data).use(),
		&exc);
	writeNode(r, node, exc);
}

// CDATASection createCDATASection(in DOMString data) raises(DOMException);
static void _createCDATASection(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& data=params.as_string(0, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createCDATASection(
		vdoc.get_document(),
		r.transcode(data).use(),
		&exc);
	writeNode(r, node, exc);
}

// ProcessingInstruction createProcessingInstruction(in DOMString target,in DOMString data) raises(DOMException);
static void _createProcessingInstruction(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& target=params.as_string(0, "data must be string");
	const String& data=params.as_string(1, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createProcessingInstruction(
		vdoc.get_document(),
		r.transcode(target).use(), 
		r.transcode(data).use(),
		&exc);
	writeNode(r, node, exc);
}

// Attr createAttribute(in DOMString name) raises(DOMException);
static void _createAttribute(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& name=params.as_string(0, "name must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createAttribute(
		vdoc.get_document(),
		r.transcode(name).use(),
		&exc);
	writeNode(r, node, exc);
}

// Attr createAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
static void _createAttributeNS(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// namespaceURI;qualifiedName
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& qualifiedName=params.as_string(1, "name must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createAttributeNS(
		vdoc.get_document(),
		r.transcode(namespaceURI).use(),
		r.transcode(qualifiedName).use(),
		&exc);
	writeNode(r, node, exc);
}

// EntityReference createEntityReference(in DOMString name) raises(DOMException);
static void _createEntityReference(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& name=params.as_string(0, "name must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createEntityReference(
		vdoc.get_document(),
		r.transcode(name).use(),
		&exc);
	writeNode(r, node, exc);
}

// NodeList getElementsByTagName(in DOMString name);
static void _getElementsByTagName(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	const String& name=params.as_string(0, "name must be string");

	VHash& result=*new VHash;
	GdomeException exc;
	if(GdomeNodeList *nodes=
		gdome_doc_getElementsByTagName(
			vdoc.get_document(), 
			r.transcode(name).use(), 
			&exc)) {
		gulong length=gdome_nl_length(nodes, &exc);
		for(gulong i=0; i<length; i++)
			result.hash().put(
				String::Body::Format(i), 
				new VXnode(&r.charsets, gdome_nl_item(nodes, i, &exc)));
	} else if(exc)
		throw Exception(0, exc);

	// write out result
	r.write_no_lang(result);
}

static void _getElementsByTagNameNS(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// namespaceURI;localName
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& localName=params.as_string(1, "localName must be string");

	GdomeException exc;
	VHash& result=*new VHash;
	if(GdomeNodeList *nodes=
		gdome_doc_getElementsByTagNameNS(
			vdoc.get_document(), 
			r.transcode(namespaceURI).use(),
			r.transcode(localName).use(),
			&exc)) {
		gulong length=gdome_nl_length(nodes, &exc);
		for(gulong i=0; i<length; i++)
			result.hash().put(
				String::Body::Format(i), 
				new VXnode(&r.charsets, gdome_nl_item(nodes, i, &exc)));
	}

	// write out result
	r.write_no_lang(result);
}

static void _getElementById(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// elementId
	const String& elementId=params.as_string(0, "elementID must be string");

	GdomeException exc;
	if(GdomeNode *node=(GdomeNode *)gdome_doc_getElementById(
		vdoc.get_document(),
		r.transcode(elementId).use(),
		&exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, node));
	} else if(exc || xmlHaveGenericErrors())
		throw Exception(
			&elementId, 
			exc);
}

static void _importNode(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	GdomeNode *importedNode=
		as_node(params, 0, "importedNode must be node");
	bool deep=
		params.as_bool(1, "deep must be bool", r);

	GdomeException exc;
	GdomeNode *outputNode=gdome_doc_importNode(vdoc.get_document(), 
		importedNode,
		deep, &exc);
	if(exc)
		throw Exception(0, exc);

	// write out result
	r.write_no_lang(*new VXnode(&r.charsets, outputNode));
}
/*
GdomeElement *gdome_doc_createElementNS (GdomeDocument *self, GdomeDOMString *namespaceURI, GdomeDOMString *qualifiedName, GdomeException *exc);
GdomeAttr *gdome_doc_createAttributeNS (GdomeDocument *self, GdomeDOMString *namespaceURI, GdomeDOMString *qualifiedName, GdomeException *exc);
*/

static void _create(Request& r, MethodParams& params) {
	Charset& source_charset=r.charsets.source();
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	Value& param=params[params.count()-1];
	GdomeDocument *document;
	bool set_encoding=false;
	if(param.get_junction()) { // {<?xml?>...}
		Temp_lang temp_lang(r, String::L_XML);
		const String& xml=r.process_to_string(param);

		const char* cstr=xml.cstr(String::L_UNSPECIFIED);
		document=(GdomeDocument *)
			gdome_xml_n_mkref((xmlNode *)xmlParseMemory(
				cstr, strlen(cstr)
			));
		//printf("document=0x%p\n", document);
		if(!document || xmlHaveGenericErrors()) {
			GdomeException exc=0;
			throw Exception(0, exc);
		}

		// must be last action in if, see after if}
	} else { // [name]
		const String& qualifiedName=param.as_string();

		GdomeException exc;
		/*
		GdomeDocumentType *documentType=gdome_di_createDocumentType (
			docimpl, 
			r.transcode(qualifiedName), 
			0/*publicId* /, 
			0/*systemId* /, 
			&exc);
		if(!documentType || exc || xmlHaveGenericErrors())
			throw Exception(
				method_name, 
				exc);
		/// +xalan createXMLDecl ?
		*/
		document=gdome_di_createDocument (domimpl, 
			0/*namespaceURI*/, 
			r.transcode(qualifiedName).use(), 
			0/*doctype*/, 
			&exc);
		if(!document || exc || xmlHaveGenericErrors())
			throw Exception(0, exc);

		set_encoding=true;
		// must be last action in if, see after if}
	}
	// must be first action after if}
	// replace any previous parsed source
	{
		vdoc.set_document(&r.charsets, document); 
		GdomeException exc;
		gdome_doc_unref(document, &exc);
	}
	
	// URI 
	const char* URI_cstr;
	const char* URI_cstr_ptr;
	if(params.count()>1) { // absolute(param)
		const String& URI=params.as_string(0, "URI must be string");
		URI_cstr=URI_cstr_ptr=r.absolute(URI).cstr();
	} else // default = disk path to requested document
		URI_cstr=r.request_info.path_translated;
	xmlDoc *doc=gdome_xml_doc_get_xmlDoc(document);
	if(URI_cstr)
		doc->URL=source_charset.transcode_buf2xchar(URI_cstr, strlen(URI_cstr));

	if(set_encoding) {
		const char* source_charset_name=source_charset.NAME().cstr();
		doc->encoding=source_charset.transcode_buf2xchar(source_charset_name, strlen(source_charset_name));
	}
}

static void _load(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// filespec
	const String& file_name=params.as_string(0, "uri must be string");
	const String& uri=r.absolute(file_name);

	File_read_result file=file_read(r.charsets, uri, false/*not text*/,
		params.count()>1?params.as_no_junction(1, "additional params must be hash")
			.get_hash()
			:0);

	GdomeDocument *document=(GdomeDocument *)
		gdome_xml_n_mkref((xmlNode *)xmlParseMemory(file.str, file.length));
	if(!document || xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw Exception(&uri, exc);
	}
	// must be first action after if}
	// replace any previous parsed source
	{
		vdoc.set_document(&r.charsets, document); 
		GdomeException exc;
		gdome_doc_unref(document, &exc);
	}

	const char* URI_cstr=uri.cstr();
	xmlDoc *doc=gdome_xml_doc_get_xmlDoc(document);
	if(URI_cstr)
		doc->URL=r.charsets.source().transcode_buf2xchar(URI_cstr, strlen(URI_cstr));

}

static void param_option_over_output_option(
					    HashStringValue& param_options, const char* option_name,
					    const String*& output_option) {
	if(Value* value=param_options.get(String::Body(option_name)))
		output_option=&value->as_string();
}
static void param_option_over_output_option(
					    HashStringValue& param_options, const char* option_name,
					    bool& output_option) {
	if(Value* value=param_options.get(String::Body(option_name))) {
		const String& s=value->as_string();
		if(s=="yes")
			output_option=true;
		else if(s=="no")
			output_option=false;
		else
			throw Exception("parser.runtime",
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
				// $.doctype-public[?]
				param_option_over_output_option(*options, "doctype-public", oo.doctypePublic);
				// $.doctype-system[?]
				param_option_over_output_option(*options, "doctype-system", oo.doctypeSystem);
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
					 const String* file_spec) {
	Xdoc2buf_result result;
	prepare_output_options(r, params, index,
		oo);

	const char* encoding_cstr=oo.encoding->cstr();
	xmlCharEncodingHandler *encoder=xmlFindCharEncodingHandler(encoding_cstr);
	if(!encoder)
		throw Exception("parser.runtime",
			0,
			"encoding '%s' not supported", encoding_cstr);
	// UTF-8 encoder contains empty input/output converters, 
	// which is wrong for xmlOutputBufferCreateIO
	// while zero encoder goes perfectly 
	if(encoder && strcmp(encoder->name, "UTF-8")==0)
		encoder=0;

	xmlOutputBuffer_auto_ptr outputBuffer(xmlAllocOutputBuffer(encoder));

	xsltStylesheet_auto_ptr stylesheet(xsltNewStylesheet());
	if(!stylesheet.get())
		throw Exception(0,
			0,
			"xsltNewStylesheet failed");

	#define OOS2STYLE(name) \
		stylesheet->name=oo.name?BAD_CAST xmlMemStrdup(r.transcode(*oo.name)->str):0
	#define OOE2STYLE(name) \
		stylesheet->name=oo.name

	OOS2STYLE(method);
	OOS2STYLE(encoding);
	OOS2STYLE(mediaType);
	OOS2STYLE(doctypeSystem);
	OOS2STYLE(doctypePublic);
	OOE2STYLE(indent);
	OOS2STYLE(version);
	OOE2STYLE(standalone);
	OOE2STYLE(omitXmlDeclaration);

	xmlDoc *document=gdome_xml_doc_get_xmlDoc(vdoc.get_document());
	if(xsltSaveResultTo(outputBuffer.get(), document, stylesheet.get())<0) {
		GdomeException exc=0;
		throw Exception(0, exc);
	}

	// write out result
	char *gnome_str;  size_t gnome_length;
	if(outputBuffer->conv) {
		gnome_length=outputBuffer->conv->use;
		gnome_str=(char *)outputBuffer->conv->content;
	} else {
		gnome_length=outputBuffer->buffer->use;
		gnome_str=(char *)outputBuffer->buffer->content;
	}

	if(file_spec)
		file_write(*file_spec,
			gnome_str, gnome_length, 
			true/*as_text*/);
	else if(result.length=gnome_length) {
		result.str=pa_strdup(gnome_str, gnome_length);
	} else
		result.str=0;

	return result;
}

static void _file(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);
	VXdoc::Output_options oo(vdoc.output_options);
	Xdoc2buf_result buf=xdoc2buf(r, vdoc, params, 0, 
		oo,
		0/*not to file, to memory*/);
	// write out result
	r.write_no_lang(String(buf.str, buf.length));

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

	const String& file_spec=r.absolute(params.as_string(0, "file name must be string"));
	
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
		0/*not to file, to memory*/);
	// write out result
	r.write_no_lang(String(buf.str, buf.length));
}

#ifndef DOXYGEN
struct Add_xslt_param_info {
	Request* r;
	Array<GdomeDOMString_auto_ptr>* strings;
	const char** current_transform_param;
};
#endif
static void add_xslt_param(
			   HashStringValue::key_type attribute, 
			   HashStringValue::value_type meaning, 
			   Add_xslt_param_info* info) {
	GdomeDOMString_auto_ptr s;
	*info->current_transform_param++=(s=info->r->transcode(attribute))->str; *info->strings+=s;
	*info->current_transform_param++=(s=info->r->transcode(meaning->as_string()))->str; *info->strings+=s;
}
static VXdoc& _transform(Request& r, const String* stylesheet_source, 
						   VXdoc& vdoc, xsltStylesheetPtr stylesheet, const char** transform_params) {
	xmlDoc *document=gdome_xml_doc_get_xmlDoc(vdoc.get_document());
	xsltTransformContext_auto_ptr transformContext(
		xsltNewTransformContext(stylesheet, document));
	// make params literal
	if (transformContext->globalVars == NULL) // strangly not initialized by xsltNewTransformContext
		transformContext->globalVars = xmlHashCreate(20);
	xsltQuoteUserParams(transformContext.get(), transform_params);
	// do transform
	xmlDoc *transformed=xsltApplyStylesheetUser(
		stylesheet,
		document,
		0/*already quoted-inserted  transform_params*/,
		0/*const char* output*/,
		0/*FILE *profile*/,
		transformContext.get());
	if(!transformed || xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw Exception(stylesheet_source, exc);
	}

	//gdome_xml_doc_mkref dislikes XML_HTML_DOCUMENT_NODE  type, fixing
	transformed->type=XML_DOCUMENT_NODE;
	// constructing result
	GdomeDocument *gdomeDocument=gdome_xml_doc_mkref(transformed);
	if(!gdomeDocument)
		throw Exception(0,
			0,
			"gdome_xml_doc_mkref failed");
	VXdoc& result=*new VXdoc(&r.charsets, gdomeDocument);
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
	oo.doctypeSystem=stylesheet->doctypeSystem?&r.transcode(stylesheet->doctypeSystem):0;
	oo.doctypePublic=stylesheet->doctypePublic?&r.transcode(stylesheet->doctypePublic):0;
	oo.indent=stylesheet->indent!=0;
	oo.version=stylesheet->version?&r.transcode(stylesheet->version):0;
	oo.standalone=stylesheet->standalone!=0;
	oo.omitXmlDeclaration=stylesheet->omitXmlDeclaration!=0;

	// return
	return result;
}
static void _transform(Request& r, MethodParams& params) {
	VXdoc& vdoc=GET_SELF(r, VXdoc);

	// params
	Array<GdomeDOMString_auto_ptr> transform_strings;
	const char** transform_params=0;
	if(params.count()>1) {
		Value& vparams=params.as_no_junction(1, "transform parameters must be hash");
		if(!vparams.is_string())
			if(HashStringValue* hash=vparams.get_hash()) {
				transform_params=new(UseGC) const char*[hash->count()*2+1];
				Add_xslt_param_info info={
					&r, 
					&transform_strings,
					transform_params
				};
				hash->for_each(add_xslt_param, &info);
				transform_params[hash->count()*2]=0;				
			} else
				throw Exception("parser.runtime",
					0,
					"transform parameters parameter must be hash");
	}

	VXdoc* result;
	if(Value *vxdoc=params[0].as(VXDOC_TYPE, false)) { // stylesheet (xdoc)
		xmlDoc *document=gdome_xml_doc_get_xmlDoc(
			static_cast<VXdoc *>(vxdoc)->get_document());
		// compile xdoc stylesheet
		xsltStylesheet_auto_ptr stylesheet_ptr(xsltParseStylesheetDoc(document)); 
		// strange thing - xsltParseStylesheetDoc records document and destroys it in stylesheet destructor
		// we don't need that
		stylesheet_ptr->doc=0;
		if(xmlHaveGenericErrors()) {
			GdomeException exc=0;
			throw Exception(0, exc);
		}
		if(!stylesheet_ptr.get())
			throw Exception("xml",
				0,
				"stylesheet failed to compile");

		// transform!
		result=&_transform(r, 0,
			vdoc, stylesheet_ptr.get(),
			transform_params);
	} else { // stylesheet (file name)
		// extablish stylesheet connection
		const String& stylesheet_filespec=
			r.absolute(params.as_string(0, "stylesheet must be file name (string) or DOM document (xdoc)"));
		Stylesheet_connection_ptr connection=stylesheet_manager.get_connection(stylesheet_filespec);

		// load and compile file to stylesheet [or get cached if any]
		// transform!
		result=&_transform(r, &stylesheet_filespec,
			vdoc, connection->stylesheet(false/*nocache*/),
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
	// NodeList getElementsByTagName(in DOMString name);
	add_native_method("getElementsByTagName", Method::CT_DYNAMIC, _getElementsByTagName, 1, 1);

	/// DOM2

	// ^xdoc.getElementById[elementId]
	add_native_method("getElementById", Method::CT_DYNAMIC, _getElementById, 1, 1);

    // Node (in Node importedNode, in boolean deep) raises(DOMException)
	add_native_method("importNode", Method::CT_DYNAMIC, _importNode, 2, 2);

	// Attr createAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
	add_native_method("createAttributeNS", Method::CT_DYNAMIC, _createAttributeNS, 2, 2);

	// Element createElementNS(in DOMString namespaceURI, in DOMString qualifiedName) raises(DOMException);
	add_native_method("createElementNS", Method::CT_DYNAMIC, _createElementNS, 2, 2);

	// NodeList getElementsByTagNameNS(in DOMString namespaceURI, in DOMString localName);
	add_native_method("getElementsByTagNameNS", Method::CT_DYNAMIC, _getElementsByTagNameNS, 2, 2);

	/// parser
	
	// ^xdoc::create{qualifiedName}
	// ^xdoc::create[<some>xml</some>]
	// ^xdoc::create[URI][<some>xml</some>]
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 2);	
	// for backward compatibility with <=v 1.82 2002/01/31 11:51:46 paf
	add_native_method("set", Method::CT_DYNAMIC, _create, 1, 1);

	// ^xdoc::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 2);

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

void MXdoc::configure_admin(Request& r) {
}

# else

#include "classes.h"

// global variable

DECLARE_CLASS_VAR(xdoc, 0, 0); // fictive

#endif

