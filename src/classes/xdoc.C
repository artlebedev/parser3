/** @file
	Parser: @b xdoc parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: xdoc.C,v 1.77 2002/01/25 11:33:46 paf Exp $
*/
#include "pa_types.h"
#ifdef XML

#include "gdome.h"
#include "libxml/tree.h"
extern "C" {
#include "gdomecore/gdome-xml-node.h"
#include "gdomecore/gdome-xml-document.h"
};
#include "libxslt/xsltInternals.h"
#include "libxslt/transform.h"
#include "libxslt/xsltutils.h"

#include "pa_stylesheet_connection.h"
#include "classes.h"
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

class MXdoc : public MXnode {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VXdoc(pool, 0); }

public:
	MXdoc(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
	void configure_admin(Request& r);
};

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

static void writeNode(Request& r,  const String& method_name, GdomeNode *node, 
					  GdomeException exc) {
	if(!node || exc)
		throw Exception(0, 0, 
			&method_name, 
			exc);

	Pool& pool=r.pool();

	// write out result
	VXnode& result=*new(pool) VXnode(pool, node);
	r.write_no_lang(result);
}

// Element createElement(in DOMString tagName) raises(DOMException);
static void _createElement(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& tagName=params->as_string(0, "tagName must be string");

	GdomeException exc;
	GdomeNode *node=
		(GdomeNode *)gdome_doc_createElement(vdoc.get_document(&method_name), 
		pool.transcode(tagName).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}

// DocumentFragment createDocumentFragment()
static void _createDocumentFragment(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	GdomeException exc;
	GdomeNode *node=
		(GdomeNode *)gdome_doc_createDocumentFragment(
		vdoc.get_document(&method_name),
		&exc);
	writeNode(r, method_name, node, exc);
}

// Text createTextNode(in DOMString data);
static void _createTextNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& data=params->as_string(0, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createTextNode(
		vdoc.get_document(&method_name),
		pool.transcode(data).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}

// Comment createComment(in DOMString data)
static void _createComment(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& data=params->as_string(0, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createComment(
		vdoc.get_document(&method_name),
		pool.transcode(data).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}

// CDATASection createCDATASection(in DOMString data) raises(DOMException);
static void _createCDATASection(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& data=params->as_string(0, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createCDATASection(
		vdoc.get_document(&method_name),
		pool.transcode(data).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}

// ProcessingInstruction createProcessingInstruction(in DOMString target,in DOMString data) raises(DOMException);
static void _createProcessingInstruction(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& target=params->as_string(0, "data must be string");
	const String& data=params->as_string(1, "data must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createProcessingInstruction(
		vdoc.get_document(&method_name),
		pool.transcode(target).get(), 
		pool.transcode(data).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}

// Attr createAttribute(in DOMString name) raises(DOMException);
static void _createAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& name=params->as_string(0, "name must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createAttribute(
		vdoc.get_document(&method_name),
		pool.transcode(name).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}
// EntityReference createEntityReference(in DOMString name) raises(DOMException);
static void _createEntityReference(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& name=params->as_string(0, "name must be string");

	GdomeException exc;
	GdomeNode *node=(GdomeNode *)gdome_doc_createEntityReference(
		vdoc.get_document(&method_name),
		pool.transcode(name).get(),
		&exc);
	writeNode(r, method_name, node, exc);
}

static void _getElementsByTagName(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& name=params->as_string(0, "name must be string");

	GdomeException exc;
	VHash& result=*new(pool) VHash(pool);
	if(GdomeNodeList *nodes=
		gdome_doc_getElementsByTagName(
			vdoc.get_document(&method_name), 
			pool.transcode(name).get(),
			&exc)) {
		gulong length=gdome_nl_length(nodes, &exc);
		for(gulong i=0; i<length; i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash(0).put(skey, new(pool) VXnode(pool, gdome_nl_item(nodes, i, &exc)));
		}
	}

	// write out result
	r.write_no_lang(result);
}

static void _getElementsByTagNameNS(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// namespaceURI;localName
	const String& namespaceURI=params->as_string(0, "namespaceURI must be string");
	const String& localName=params->as_string(0, "localName must be string");

	GdomeException exc;
	VHash& result=*new(pool) VHash(pool);
	if(GdomeNodeList *nodes=
		gdome_doc_getElementsByTagNameNS(
			vdoc.get_document(&method_name), 
			pool.transcode(namespaceURI).get(),
			pool.transcode(localName).get(),
			&exc)) {
		gulong length=gdome_nl_length(nodes, &exc);
		for(gulong i=0; i<length; i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash(0).put(skey, new(pool) VXnode(pool, gdome_nl_item(nodes, i, &exc)));
		}
	}


	// write out result
	r.write_no_lang(result);
}

static void _getElementById(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// elementId
	const String& elementId=params->as_string(0, "elementID must be string");

	GdomeException exc;
	if(GdomeNode *node=(GdomeNode *)gdome_doc_getElementById(
		vdoc.get_document(&method_name),
		pool.transcode(elementId).get(),
		&exc)) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, node);
		r.write_no_lang(result);
	} else if(exc || xmlHaveGenericErrors())
		throw Exception(0, 0, 
			&method_name, 
			exc);
}
/*
GdomeNode *gdome_doc_importNode (GdomeDocument *self, GdomeNode *importedNode, GdomeBoolean deep, GdomeException *exc);
GdomeElement *gdome_doc_createElementNS (GdomeDocument *self, GdomeDOMString *namespaceURI, GdomeDOMString *qualifiedName, GdomeException *exc);
GdomeAttr *gdome_doc_createAttributeNS (GdomeDocument *self, GdomeDOMString *namespaceURI, GdomeDOMString *qualifiedName, GdomeException *exc);
*/


static void _create(Request& r, const String& method_name, MethodParams *params) {
	//_asm int 3;
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& qualifiedName=params->as_string(0, "qualifiedName must be string");

	GdomeException exc;
	/*
	GdomeDocumentType *documentType=gdome_di_createDocumentType (
		docimpl, 
		pool.transcode(qualifiedName), 
		0/*publicId* /, 
		0/*systemId* /, 
		&exc);
	if(!documentType || exc || xmlHaveGenericErrors())
		throw Exception(0, 0, 
			&method_name, 
			exc);
	*/
	GdomeDocument *document=gdome_di_createDocument (domimpl, 
		0/*namespaceURI*/, 
		pool.transcode(qualifiedName).get(), 
		0/*doctype*/, 
		&exc);
	if(!document || exc || xmlHaveGenericErrors())
		throw Exception(0, 0, 
			&method_name, 
			exc);

	/// +xalan createXMLDecl ?

	// replace any previous parsed source
	vdoc.set_document(document);
}

static void _set(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	Value& vxml=params->as_junction(0, "xml must be code");
	Temp_lang temp_lang(r, String::UL_XML);
	const String& xml=r.process(vxml).as_string();

	GdomeDocument *document=(GdomeDocument *)
		gdome_xml_n_mkref((xmlNode *)xmlParseMemory(
			xml.cstr(String::UL_AS_IS), xml.size()
		));
	if(!document || xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw Exception(0, 0, 
			&method_name, 
			exc);
	}

	// replace any previous parsed source
	vdoc.set_document(document);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "uri must be string");
	const String& uri=r.absolute(file_name);
	
	GdomeDocument *document=(GdomeDocument *)
		gdome_xml_n_mkref((xmlNode *)xmlParseFile(uri.cstr()));
	if(!document || xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw Exception(0, 0, 
			&uri, 
			exc);
	}

	// replace any previous parsed source
	vdoc.set_document(document);
}

static void param_option_over_output_option(Pool& pool, 
											Hash *param_options, const char *option_name,
											const String *& output_option) {
	if(Value *value=static_cast<Value *>(param_options->get(*new(pool) String(pool, 
		option_name))))
		output_option=&value->as_string();
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

static void prepare_output_options(
								   Pool& pool, const String& method_name, MethodParams *params, int index,
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
	if(params->size()>index) {
		Value& voptions=params->as_no_junction(index, "options must be string");
		if(voptions.is_defined()) {
			if(Hash *options=voptions.get_hash(&method_name)) {
				// $.method[xml|html|text]
				if(Value *vmethod=static_cast<Value *>(options->get(*new(pool) 
					String(pool, XDOC_OUTPUT_METHOD_OPTION_NAME))))
					oo.method=&vmethod->as_string();

				// $.version[1.0]
				param_option_over_output_option(pool, options, "version", oo.version);
				// $.encoding[windows-1251|...]
				param_option_over_output_option(pool, options, "encoding", oo.encoding);
				// $.omit-xml-declaration[yes|no]
				param_option_over_output_option(pool, options, "omit-xml-declaration", oo.omitXmlDeclaration);
				// $.standalone[yes|no]
				param_option_over_output_option(pool, options, "standalone", oo.standalone);
				// $.doctype-public[?]
				param_option_over_output_option(pool, options, "doctype-public", oo.doctypePublic);
				// $.doctype-system[?]
				param_option_over_output_option(pool, options, "doctype-system", oo.doctypeSystem);
				// $.indent[yes|no]
				param_option_over_output_option(pool, options, "indent", oo.indent);
				// $.media-type[text/{html|xml|plain}]
				param_option_over_output_option(pool, options, "media-type", oo.mediaType);				 
			}
		}
	}

	// default encoding from pool
	if(!oo.encoding)
		oo.encoding=&pool.get_source_charset().name();
	// default method=xml
	if(!oo.method)
		oo.method=new(pool) String(pool, XDOC_OUTPUT_METHOD_OPTION_VALUE_XML);
	// default mediaType = depending on method
	if(!oo.mediaType) {
		if(*oo.method==XDOC_OUTPUT_METHOD_OPTION_VALUE_XML)
			oo.mediaType=new(pool) String(pool, "text/xml");
		else if(*oo.method==XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML)
			oo.mediaType=new(pool) String(pool, "text/html");
		else // XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT & all others
			oo.mediaType=new(pool) String(pool, "text/plain");
	}
}

static void xdoc2buf(Pool& pool, VXdoc& vdoc, 
					 const String& method_name, MethodParams *params, int index,
					 VXdoc::Output_options& oo,
					 const String *file_spec,
					 char **parser_buf, size_t *parser_size) {
	prepare_output_options(pool, method_name, params, index,
		oo);

	const char *encoding_cstr=oo.encoding->cstr();
	xmlCharEncodingHandler *encoder=xmlFindCharEncodingHandler(encoding_cstr);
	if(!encoder)
		throw Exception(0, 0,
			&method_name,
			"encoding '%s' not supported", encoding_cstr);
	// UTF-8 encoder contains empty input/output converters, 
	// which is wrong for xmlOutputBufferCreateIO
	// while zero encoder goes perfectly 
	if(encoder && strcmp(encoder->name, "UTF-8")==0)
		encoder=0;

	xmlOutputBuffer_auto_ptr outputBuffer(xmlAllocOutputBuffer(encoder));

	xsltStylesheet_auto_ptr stylesheet(xsltNewStylesheet());
	if(!stylesheet.get())
		throw Exception(0, 0,
			&method_name,
			"xsltNewStylesheet failed");

	#define OOS2STYLE(name) \
		stylesheet->name=oo.name?BAD_CAST g_strdup(pool.transcode(*oo.name)->str):0
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

	xmlDoc *document=gdome_xml_doc_get_xmlDoc(vdoc.get_document(&method_name));
	if(xsltSaveResultTo(outputBuffer.get(), document, stylesheet.get())<0) {
		GdomeException exc=0;
		throw Exception(0, 0, 
			&method_name, 
			exc);
	}

	// write out result
	char *gnome_buf;  size_t gnome_size;
	if(outputBuffer->conv) {
		gnome_size=outputBuffer->conv->use;
		gnome_buf=(char *)outputBuffer->conv->content;
	} else {
		gnome_size=outputBuffer->buffer->use;
		gnome_buf=(char *)outputBuffer->buffer->content;
	}
	if(file_spec)
		file_write(
					*file_spec,
					gnome_buf, gnome_size, 
					true/*as_text*/);
	else if(*parser_size=gnome_size) {
		*parser_buf=(char *)pool.malloc(gnome_size);
		memcpy(*parser_buf, gnome_buf, gnome_size);
	} else
		*parser_buf=0;
}

static void _file(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);
	VXdoc::Output_options oo(vdoc.output_options);
	char *buf; size_t buf_size;
	xdoc2buf(pool, vdoc, method_name, params, 0, 
		oo,
		0/*not to file, to memory*/,
		&buf, &buf_size);
	// write out result
	r.write_no_lang(*new(pool) String(pool, buf, buf_size));

	// write out result
	VFile& vfile=*new(pool) VFile(pool);
	Value *vcontent_type;
	VHash *vhcontent_type=new(pool) VHash(pool);
	vhcontent_type->hash(&method_name).put(
		*value_name, 
		new(pool) VString(*oo.mediaType));
	vhcontent_type->hash(&method_name).put(
		*new(pool) String(pool, "charset"), 
		new(pool) VString(*oo.encoding));
	vcontent_type=vhcontent_type;
	
	vfile.set(false/*tainted*/, buf, buf_size, 0/*file_name*/, vcontent_type);
	r.write_no_lang(vfile);
}

static void _save(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	const String& file_spec=r.absolute(params->as_string(0, "file name must be string"));
	
	VXdoc::Output_options oo(vdoc.output_options);
	xdoc2buf(pool, vdoc, method_name, params, 1, 
		oo,
		&file_spec,
		0, 0);
}

static void _string(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);
	VXdoc::Output_options oo(vdoc.output_options);
	char *buf; size_t buf_size;
	xdoc2buf(pool, vdoc, method_name, params, 0, 
		oo,
		0/*not to file, to memory*/,
		&buf, &buf_size);
	// write out result
	r.write_no_lang(*new(pool) String(pool, buf, buf_size));
}

static void add_xslt_param(const Hash::Key& aattribute, Hash::Val *ameaning, 
						   void *info) {
	Value *meaning=static_cast<Value *>(ameaning);
	Pool& pool=meaning->pool();
	const char **transform_params=(const char **)info;
	*transform_params++=pool.transcode(aattribute)->str;
	*transform_params++=pool.transcode(meaning->as_string())->str;
}
static void _transform(Request& r, const String& method_name, MethodParams *params) {
	//_asm int 3;
	Pool& pool=r.pool();
	VXdoc& vdoc=*static_cast<VXdoc *>(r.self);

	// params
	const char **transform_params=0;
	if(params->size()>1) {
		Value& vparams=params->as_no_junction(1, "transform parameters must be hash");
		if(vparams.is_defined())
			if(Hash *params=vparams.get_hash(&method_name)) {
				transform_params=
					(const char **)pool.malloc(sizeof(const char *)*params->size()*2+1);
				params->for_each(add_xslt_param, transform_params);
				transform_params[params->size()*2]=0;				
			} else
				throw Exception(0, 0,
					&method_name,
					"transform parameters parameter must be hash");
	}

	// stylesheet
	const String& stylesheet_filespec=r.absolute(params->as_string(0, "file name must be string"));
	Stylesheet_connection_ptr connection=stylesheet_manager->get_connection(stylesheet_filespec);

	// transform
	xsltStylesheet *stylesheet=connection->stylesheet(false/*nocache*/);
	xmlDoc *document=gdome_xml_doc_get_xmlDoc(vdoc.get_document(&method_name));
	xsltTransformContext_auto_ptr transformContext(
		xsltNewTransformContext(stylesheet, document));
	xmlDoc *transformed=xsltApplyStylesheetUser(
		stylesheet,
		document,
		transform_params,
		0/*const char *output*/,
		0/*FILE *profile*/,
		transformContext.get());
	if(!transformed || xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw Exception(0, 0,
			&stylesheet_filespec, 
			exc);
	}

	//gdome_xml_doc_mkref dislikes XML_HTML_DOCUMENT_NODE  type, fixing
	transformed->type=XML_DOCUMENT_NODE;
	// constructing result
	GdomeDocument *gdomeDocument=gdome_xml_doc_mkref(transformed);
	if(!gdomeDocument)
		throw Exception(0, 0,
			&method_name,
			"gdome_xml_doc_mkref failed");
	VXdoc& result=*new(pool) VXdoc(pool, gdomeDocument);
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
	memset(&result.output_options, 0, sizeof(result.output_options));
	VXdoc::Output_options& oo=result.output_options;

	oo.method=stylesheet->method?&pool.transcode(stylesheet->method):0;
	oo.encoding=stylesheet->encoding?&pool.transcode(stylesheet->encoding):0;
	oo.mediaType=stylesheet->mediaType?&pool.transcode(stylesheet->mediaType):0;
	oo.doctypeSystem=stylesheet->doctypeSystem?&pool.transcode(stylesheet->doctypeSystem):0;
	oo.doctypePublic=stylesheet->doctypePublic?&pool.transcode(stylesheet->doctypePublic):0;
	oo.indent=stylesheet->indent!=0;
	oo.version=stylesheet->version?&pool.transcode(stylesheet->version):0;
	oo.standalone=stylesheet->standalone!=0;
	oo.omitXmlDeclaration=stylesheet->omitXmlDeclaration!=0;

	// write out result
	r.write_no_lang(result);
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
	add_native_method("getElementsByTagName", Method::CT_DYNAMIC, _getElementsByTagName, 1, 1);
	// ^xdoc.getElementsByTagNameNS[namespaceURI;localName] = array of nodes
	add_native_method("getElementsByTagNameNS", Method::CT_DYNAMIC, _getElementsByTagNameNS, 2, 2);

	/// DOM2(?)

	// ^xdoc.getElementById[elementId]
	add_native_method("getElementById", Method::CT_DYNAMIC, _getElementById, 1, 1);

	/// parser
	
	// ^xdoc::create{qualifiedName}
	add_native_method("create", Method::CT_DYNAMIC, _create, 1, 1);	
	// ^xdoc::set[<some>xml</some>]
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 1);

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
