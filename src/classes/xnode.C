/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: xnode.C,v 1.22 2002/01/11 12:24:27 paf Exp $
*/
#include "classes.h"
#ifdef XML

#include "pa_charset.h"
#include "pa_request.h"
#include "pa_vxnode.h"

#include "xnode.h"

extern "C" {
#include "gdomecore/gdome-xml-node.h"
};
#include "gdomecore/gdome-xml-document.h"
#include "gdome.h"
#include "libxml/xpath.h"

// classes

class xmlXPathObject_auto_ptr {
public:
	explicit xmlXPathObject_auto_ptr(xmlXPathObject *_P = 0) 
		: _Owns(_P != 0), _Ptr(_P) {}
	xmlXPathObject_auto_ptr(const xmlXPathObject_auto_ptr& _Y) 
		: _Owns(_Y._Owns), _Ptr(_Y.release()) {}
	xmlXPathObject_auto_ptr& operator=(const xmlXPathObject_auto_ptr& _Y) 
		{if (this != &_Y)
			{if (_Ptr != _Y.get())
				{if (_Owns && _Ptr)
					xmlXPathFreeObject(_Ptr);
				_Owns = _Y._Owns; }
			else if (_Y._Owns)
				_Owns = true;
			_Ptr = _Y.release(); }
		return (*this); }
	~xmlXPathObject_auto_ptr()
		{if (_Owns && _Ptr)
			xmlXPathFreeObject(_Ptr); }
	xmlXPathObject& operator*() const 
		{return (*get()); }
	xmlXPathObject *operator->() const 
		{return (get()); }
	xmlXPathObject *get() const 
		{return (_Ptr); }
	xmlXPathObject *release() const 
		{((xmlXPathObject_auto_ptr *)this)->_Owns = false;
		return (_Ptr); }
private:
	bool _Owns;
	xmlXPathObject *_Ptr;
};

class xmlXPathContext_auto_ptr {
public:
	explicit xmlXPathContext_auto_ptr(xmlXPathContext *_P = 0) 
		: _Owns(_P != 0), _Ptr(_P) {}
	xmlXPathContext_auto_ptr(const xmlXPathContext_auto_ptr& _Y) 
		: _Owns(_Y._Owns), _Ptr(_Y.release()) {}
	xmlXPathContext_auto_ptr& operator=(const xmlXPathContext_auto_ptr& _Y) 
		{if (this != &_Y)
			{if (_Ptr != _Y.get())
				{if (_Owns && _Ptr)
					xmlXPathFreeContext(_Ptr);
				_Owns = _Y._Owns; }
			else if (_Y._Owns)
				_Owns = true;
			_Ptr = _Y.release(); }
		return (*this); }
	~xmlXPathContext_auto_ptr()
		{if (_Owns && _Ptr)
			xmlXPathFreeContext(_Ptr); }
	xmlXPathContext& operator*() const 
		{return (*get()); }
	xmlXPathContext *operator->() const 
		{return (get()); }
	xmlXPathContext *get() const 
		{return (_Ptr); }
	xmlXPathContext *release() const 
		{((xmlXPathContext_auto_ptr *)this)->_Owns = false;
		return (_Ptr); }
private:
	bool _Owns;
	xmlXPathContext *_Ptr;
};

// defines

#define XNODE_CLASS_NAME "xnode"

// helpers

GdomeNode *as_node(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	Value& value=params->as_no_junction(index, msg);
	if(strcmp(value.type(), VXNODE_TYPE)!=0)
		throw Exception(0, 0,
			&method_name,
			msg);

	VXnode& vnode=*static_cast<VXnode *>(&value);
	return vnode.get_node(&method_name);
}

// helpers

GdomeAttr * as_attr(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	GdomeNode *node=as_node(pool, method_name, params, index, msg);
	GdomeException exc;
	if(gdome_n_nodeType(node, &exc)!=GDOME_ATTRIBUTE_NODE)
		throw Exception(0, 0, 
			&method_name,
			msg);

	return GDOME_A(node);
}

// methods

// DOM1 node

// Node insertBefore(in Node newChild,in Node refChild) raises(DOMException);
static void _insertBefore(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *newChild=as_node(pool, method_name, params, 0, "newChild must be node");
	GdomeNode *refChild=as_node(pool, method_name, params, 1, "refChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_insertBefore(selfNode, newChild, refChild, &exc)) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, retNode);
		r.write_no_lang(result);		
	} else
		throw Exception(0, 0, 
			&method_name, 
			exc);
}

// Node replaceChild(in Node newChild,in Node oldChild) raises(DOMException);
static void _replaceChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *newChild=as_node(pool, method_name, params, 0, "newChild must be node");
	GdomeNode *refChild=as_node(pool, method_name, params, 1, "refChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_replaceChild(selfNode, newChild, refChild, &exc)) {
		// write out result
		r.write_no_lang(*new(pool) VXnode(pool, retNode));		
	} else
		throw Exception(0, 0, 
			&method_name, 
			exc);
}

// Node removeChild(in Node oldChild) raises(DOMException);
static void _removeChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *oldChild=as_node(pool, method_name, params, 0, "oldChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_removeChild(selfNode, oldChild, &exc)) {
		// write out result
		r.write_no_lang(*new(pool) VXnode(pool, retNode));
	} else
		throw Exception(0, 0, 
			&method_name, 
			exc);
}

// Node appendChild(in Node newChild) raises(DOMException);
static void _appendChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *newChild=as_node(pool, method_name, params, 0, "newChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_appendChild(selfNode, newChild, &exc)) {
		// write out result
		r.write_no_lang(*new(pool) VXnode(pool, retNode));		
	}  else
		throw Exception(0, 0, 
			&method_name, 
			exc);
}

// boolean hasChildNodes();
static void _hasChildNodes(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *node=vnode.get_node(&method_name);

	GdomeException exc;
	// write out result
	VBool& result=*new(pool) VBool(pool, gdome_n_hasChildNodes(node, &exc)!=0);
	result.set_name(method_name);
	r.write_no_lang(result);
}

// Node cloneNode(in boolean deep);
/// @test ownerDocument=?
static void _cloneNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *node=vnode.get_node(&method_name);

	bool deep=params->as_bool(0, "deep must be bool", r);

	GdomeException exc;
	// write out result
	VXnode& result=*new(pool) VXnode(pool, gdome_n_cloneNode(node, deep, &exc));
	result.set_name(method_name);
	r.write_no_lang(result);
}

// DOM1 element

GdomeElement *get_self_element(Request& r, const String& method_name) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *node=vnode.get_node(&method_name);

	GdomeException exc;
	if(gdome_n_nodeType(node, &exc)!=GDOME_ELEMENT_NODE)
		throw Exception(0, 0, 
			&method_name,
			"method can be called on node of ELEMENT type");

	return GDOME_EL(node);
}

// DOMString getAttribute(in DOMString name);
static void _getAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");

	GdomeException exc;
	GdomeDOMString *attribute_value=
		gdome_el_getAttribute(element, pool.transcode(name).get(), &exc);
	// write out result
	r.write_no_lang(*new(pool) VString(pool.transcode(attribute_value)));
}

// void setAttribute(in DOMString name, in DOMString value) raises(DOMException);
static void _setAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");
	const String& attribute_value=params->as_string(1, "value must be string");

	GdomeException exc;
	gdome_el_setAttribute(element,
		pool.transcode(name).get(), 
		pool.transcode(attribute_value).get(),
		&exc);
	if(exc)
		throw Exception(0, 0, 
			&method_name, 
			exc);
}

// void removeAttribute(in DOMString name) raises(DOMException);
static void _removeAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");

	GdomeException exc;
	gdome_el_removeAttribute(element, pool.transcode(name).get(), &exc);
	if(exc)
		throw Exception(0, 0, 
			&method_name, 
			exc);
}

// Attr getAttributeNode(in DOMString name);
static void _getAttributeNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");

	GdomeException exc;
	if(GdomeAttr *attr=gdome_el_getAttributeNode(element, pool.transcode(name).get(), &exc)) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, (GdomeNode *)attr);
		r.write_no_lang(result);
	} else if(exc)
		throw Exception(0, 0, 
			&method_name, 
			exc);
}	

// Attr setAttributeNode(in Attr newAttr) raises(DOMException);
static void _setAttributeNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);
	GdomeAttr * newAttr=as_attr(pool, method_name, params, 0, "newAttr must be ATTRIBUTE node");

	GdomeException exc;
	if(GdomeAttr *returnAttr=gdome_el_setAttributeNode(element, newAttr, &exc)) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, (GdomeNode *)returnAttr);
		r.write_no_lang(result);
	} else
		throw Exception(0, 0, 
			&method_name, 
			exc);
}	

// Attr removeAttributeNode(in Attr oldAttr) raises(DOMException);
static void _removeAttributeNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);
	GdomeAttr * oldAttr=as_attr(pool, method_name, params, 0, "oldAttr must be ATTRIBUTE node");

	GdomeException exc;
	gdome_el_removeAttributeNode(element, oldAttr, &exc);
	if(exc)
		throw Exception(0, 0, 
		&method_name, 
		exc);
}	

// NodeList getElementsByTagName(in DOMString name);
static void _getElementsByTagName(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);

	const String& name=params->as_string(0, "name must be string");

	VHash& result=*new(pool) VHash(pool);
	GdomeException exc;
	if(GdomeNodeList *nodes=
		gdome_el_getElementsByTagName(element, pool.transcode(name).get(), &exc)) {
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
	} else if(exc)
		throw Exception(0, 0, 
		&method_name, 
		exc);

	// write out result
	r.write_no_lang(result);
}

// void normalize();
static void _normalize(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);

	GdomeException exc;
	gdome_n_normalize(selfNode, &exc);
	if(exc)
		throw Exception(0, 0, 
		&method_name, 
		exc);
}
/*
static void _select(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must be string");
	const char *expression_cstr=expression.cstr();
	XalanDOMString dstring(expression_cstr);
	const XalanDOMChar *expression_dcstr=dstring.c_str();

	XPathEvaluator evaluator;
	// We'll use these to parse the XML file.
	XalanSourceTreeDOMSupport dom_support;

	try {
		NodeRefList list=evaluator.selectNodeList(dom_support, 
			&vnode.get_node(&method_name), 
			expression_dcstr);

		VHash& result=*new(pool) VHash(pool);
		for(int i=0; i<list.getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash(0).put(skey, new(pool) VXnode(pool, list.item(i), false));
		}
		result.set_name(method_name);
		r.write_no_lang(result);
	} catch(const XSLException& e) {
		Exception::provide_source(pool, &expression, e);
	}
}
*/
static void _selectSingle(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must be string");

	GdomeException exc;
	GdomeNode *dome_node=vnode.get_node(&method_name);
	GdomeDocument *dome_document=gdome_n_ownerDocument(dome_node, &exc);
	if(!dome_document)
		dome_document=GDOME_DOC(dome_node);
	xmlDoc *xml_document=((_Gdome_xml_Document *)dome_document)->n;
    xmlXPathContext_auto_ptr ctxt(xmlXPathNewContext(xml_document));
	ctxt->node=xmlDocGetRootElement(xml_document);
	/*error to stderr for now*/
	xmlXPathObject_auto_ptr res(
		xmlXPathEvalExpression(BAD_CAST pool.transcode(expression)->str, ctxt.get()));

   	if(res.get())
		switch(res->type) {
		case XPATH_UNDEFINED: break;
		case XPATH_NODESET: 
			{
				if(res->nodesetval->nodeNr>1)
					throw Exception(0, 0,
					&expression,
					"resulted not in a single node (%d)", res->nodesetval->nodeNr);
				
				VXnode& result=*new(pool) VXnode(
					pool, 
					gdome_xml_n_mkref(res->nodesetval->nodeTab[0]));
				result.set_name(method_name);
				r.write_no_lang(result);
				break;
			}
		//case XPATH_BOOLEAN: nothing; break;
		//case XPATH_NUMBER: nothing; break;
		//case XPATH_STRING: nothing; break;
		default: 
			throw Exception(0, 0,
				&expression,
				"unrecognized xmlXPathEvalExpression result type (%d)", res->type);
			break; // never
		}
}

// constructor

MXnode::MXnode(Pool& apool) : Methoded(apool), 
	consts(apool) {
	set_name(*NEW String(pool(), XNODE_CLASS_NAME));

	/// DOM1 node

	// Node insertBefore(in Node newChild,in Node refChild) raises(DOMException);
	add_native_method("insertBefore", Method::CT_DYNAMIC, _insertBefore, 2, 2);
	// Node replaceChild(in Node newChild,in Node oldChild) raises(DOMException);
	add_native_method("replaceChild", Method::CT_DYNAMIC, _replaceChild, 2, 2);
	// Node removeChild(in Node oldChild) raises(DOMException);
	add_native_method("removeChild", Method::CT_DYNAMIC, _removeChild, 1, 1);
	// Node appendChild(in Node newChild) raises(DOMException);
	add_native_method("appendChild", Method::CT_DYNAMIC, _appendChild, 1, 1);
	// boolean hasChildNodes();
	add_native_method("hasChildNodes", Method::CT_DYNAMIC, _hasChildNodes, 0, 0);
	// Node cloneNode(in boolean deep);
	add_native_method("cloneNode", Method::CT_DYNAMIC, _cloneNode, 1, 1);

	/// DOM1 element

	// DOMString getAttribute(in DOMString name);
	add_native_method("getAttribute", Method::CT_DYNAMIC, _getAttribute, 1, 1);
	// void setAttribute(in DOMString name, in DOMString value) raises(DOMException);
	add_native_method("setAttribute", Method::CT_DYNAMIC, _setAttribute, 2, 2);
	// void removeAttribute(in DOMString name) raises(DOMException);
	add_native_method("removeAttribute", Method::CT_DYNAMIC, _removeAttribute, 1, 1);
	// Attr getAttributeNode(in DOMString name);
	add_native_method("getAttributeNode", Method::CT_DYNAMIC, _getAttributeNode, 1, 1);
	// Attr setAttributeNode(in Attr newAttr) raises(DOMException);
	add_native_method("setAttributeNode", Method::CT_DYNAMIC, _setAttributeNode, 1, 1);
	// Attr removeAttributeNode(in Attr oldAttr) raises(DOMException);
	add_native_method("removeAttributeNode", Method::CT_DYNAMIC, _removeAttributeNode, 1, 1);
	// NodeList getElementsByTagName(in DOMString name);
	add_native_method("getElementsByTagName", Method::CT_DYNAMIC, _getElementsByTagName, 1, 1);
	// void normalize();
	add_native_method("normalize", Method::CT_DYNAMIC, _normalize, 0, 0);

	/// parser
	// ^node.select[/some/xpath/query] = hash $.#[dnode]
//	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 1);

	// ^node.selectSingle[/some/xpath/query] = first dnode
	add_native_method("selectSingle", Method::CT_DYNAMIC, _selectSingle, 1, 1);

	// consts

#define CONST(name) \
	consts.put(*new(pool()) String(pool(), #name), new(pool()) VInt(pool(), GDOME_##name))

	CONST(ELEMENT_NODE);
    CONST(ATTRIBUTE_NODE);
    CONST(TEXT_NODE);
    CONST(CDATA_SECTION_NODE);
    CONST(ENTITY_REFERENCE_NODE);
    CONST(ENTITY_NODE);
    CONST(PROCESSING_INSTRUCTION_NODE);
    CONST(COMMENT_NODE);
    CONST(DOCUMENT_NODE);
    CONST(DOCUMENT_TYPE_NODE);
    CONST(DOCUMENT_FRAGMENT_NODE);
    CONST(NOTATION_NODE);

}

// global variable

Methoded *Xnode_class;

#endif

// creator
Methoded *MXnode_create(Pool& pool) {
	return 
#ifdef XML
		Xnode_class=new(pool) MXnode(pool)
#else
		0
#endif
		;
}
