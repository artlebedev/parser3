/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "classes.h"
#ifdef XML

static const char* IDENT_XNODE_C="$Id: xnode.C,v 1.46 2002/08/01 11:26:44 paf Exp $";

#include "pa_charset.h"
#include "pa_request.h"
#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vvoid.h"

#include "xnode.h"

extern "C" {
#include "gdomecore/gdome-xml-node.h"
#include "gdomecore/gdome-xml-document.h"
};
#include "gdome.h"
#include "libxml/xpath.h"

// classes

class xmlXPathObject_auto_ptr {
public:
	explicit xmlXPathObject_auto_ptr(xmlXPathObject *_APtr = 0) 
		: _Owns(_APtr != 0), _Ptr(_APtr) {}
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
	explicit xmlXPathContext_auto_ptr(xmlXPathContext *_APtr = 0) 
		: _Owns(_APtr != 0), _Ptr(_APtr) {}
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

// helpers

GdomeNode *as_node(const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	Value& value=params->as_no_junction(index, msg);
	if(!(strcmp(value.type(), VXNODE_TYPE)==0 
		|| strcmp(value.type(), VXDOC_TYPE)==0))
		throw Exception("parser.runtime",
			&method_name,
			msg);

	VXnode& vnode=*static_cast<VXnode *>(&value);
	return vnode.get_node(&method_name);
}

// helpers

GdomeAttr * as_attr(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	GdomeNode *node=as_node(method_name, params, index, msg);
	GdomeException exc;
	if(gdome_n_nodeType(node, &exc)!=GDOME_ATTRIBUTE_NODE)
		throw Exception("parser.runtime",
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
	GdomeNode *newChild=as_node(method_name, params, 0, "newChild must be node");
	GdomeNode *refChild=as_node(method_name, params, 1, "refChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_insertBefore(selfNode, newChild, refChild, &exc)) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, retNode);
		r.write_no_lang(result);		
	} else
		throw Exception(
			&method_name, 
			exc);
}

// Node replaceChild(in Node newChild,in Node oldChild) raises(DOMException);
static void _replaceChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *newChild=as_node(method_name, params, 0, "newChild must be node");
	GdomeNode *refChild=as_node(method_name, params, 1, "refChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_replaceChild(selfNode, newChild, refChild, &exc)) {
		// write out result
		r.write_no_lang(*new(pool) VXnode(pool, retNode));		
	} else
		throw Exception(
			&method_name, 
			exc);
}

// Node removeChild(in Node oldChild) raises(DOMException);
static void _removeChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *oldChild=as_node(method_name, params, 0, "oldChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_removeChild(selfNode, oldChild, &exc)) {
		// write out result
		r.write_no_lang(*new(pool) VXnode(pool, retNode));
	} else
		throw Exception(
			&method_name, 
			exc);
}

// Node appendChild(in Node newChild) raises(DOMException);
static void _appendChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);
	GdomeNode *newChild=as_node(method_name, params, 0, "newChild must be node");
	
	GdomeException exc;
	if(GdomeNode *retNode=gdome_n_appendChild(selfNode, newChild, &exc)) {
		// write out result
		r.write_no_lang(*new(pool) VXnode(pool, retNode));		
	}  else
		throw Exception(
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
	bool result=gdome_n_hasChildNodes(node, &exc)!=0;
	r.write_no_lang(*new(pool) VBool(pool, result));
}

// Node cloneNode(in boolean deep);
static void _cloneNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *node=vnode.get_node(&method_name);

	bool deep=params->as_bool(0, "deep must be bool", r);

	GdomeException exc;
	// write out result
	r.write_no_lang(*new(pool) VXnode(pool, gdome_n_cloneNode(node, deep, &exc)));
}

// DOM1 element

GdomeElement *get_self_element(Request& r, const String& method_name) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *node=vnode.get_node(&method_name);

	GdomeException exc;
	if(gdome_n_nodeType(node, &exc)!=GDOME_ELEMENT_NODE)
		throw Exception("parser.runtime",
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
	r.write_no_lang(pool.transcode(attribute_value));
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
		throw Exception(
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
		throw Exception(
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
		throw Exception(
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
		throw Exception(
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
		throw Exception(
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
		throw Exception(
			&method_name, 
			exc);

	// write out result
	r.write_no_lang(result);
}

static void _getElementsByTagNameNS(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	GdomeElement *element=get_self_element(r, method_name);

	// namespaceURI;localName
	const String& namespaceURI=params->as_string(0, "namespaceURI must be string");
	const String& localName=params->as_string(1, "localName must be string");

	GdomeException exc;
	VHash& result=*new(pool) VHash(pool);
	if(GdomeNodeList *nodes=
		gdome_el_getElementsByTagNameNS(
			element, 
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

// void normalize();
static void _normalize(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	GdomeNode *selfNode=vnode.get_node(&method_name);

	GdomeException exc;
	gdome_n_normalize(selfNode, &exc);
	if(exc)
		throw Exception(
			&method_name, 
			exc);
}

static void _selectX(Request& r, const String& method_name, MethodParams *params,
					 void (*handler)(Pool& pool,
							  const String& expression, 
							  xmlXPathObject_auto_ptr res,
							  Value *& result)) {
//	_asm int 3;
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must be string");

	GdomeException exc;
	GdomeNode *dome_node=vnode.get_node(&method_name);
	GdomeDocument *dome_document=gdome_n_ownerDocument(dome_node, &exc);
	if(!dome_document) // document does not own itself, so ownerDocument = 0
		dome_document=GDOME_DOC(dome_node); // and we need downcast
	xmlDoc *xml_document=gdome_xml_doc_get_xmlDoc(dome_document);
    xmlXPathContext_auto_ptr ctxt(xmlXPathNewContext(xml_document));
	ctxt->node=gdome_xml_n_get_xmlNode(dome_node);
	/*error to stderr for now*/
	xmlXPathObject_auto_ptr res(
		xmlXPathEvalExpression(BAD_CAST pool.transcode(expression)->str, ctxt.get()));

	if(xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw Exception(&expression, exc);
	}

	Value *result=0;
   	if(res.get())
		handler(pool, expression, res, result);
	if(result)
		r.write_no_lang(*result);
}

static void selectNodesHandler(Pool& pool,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  Value *& result) {
	VHash *vhash=new(pool) VHash(pool);  result=vhash;
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_NODESET:
		if(res->nodesetval)
			if(int size=res->nodesetval->nodeNr) {
				Hash& hash=vhash->hash(0);
				for(int i=0; i<size; i++) {
					String& skey=*new(pool) String(pool);
					{
						char *buf=(char *)pool.malloc(MAX_NUMBER);
						snprintf(buf, MAX_NUMBER, "%d", i);
						skey << buf;
					}

					hash.put(skey, new(pool) VXnode(pool, 
						gdome_xml_n_mkref(res->nodesetval->nodeTab[i])));
				}
			}
		break;
	default: 
		throw Exception(0,
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectNodeHandler(Pool& pool,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  Value *& result) {
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_NODESET: 
		if(res->nodesetval && res->nodesetval->nodeNr) { // empty result strangly has NODESET  res->type
			if(res->nodesetval->nodeNr>1)
				throw Exception("parser.runtime",
				&expression,
				"resulted not in a single node (%d)", res->nodesetval->nodeNr);
			
			result=new(pool) VXnode(pool, gdome_xml_n_mkref(res->nodesetval->nodeTab[0]));
		}
		break;
	case XPATH_BOOLEAN: 
		result=new(pool) VBool(pool, res->boolval!=0);
		break;
	case XPATH_NUMBER: 
		result=new(pool) VDouble(pool, res->floatval);
		break;
	case XPATH_STRING:
		result=new(pool) VString(
			pool.transcode(
				GdomeDOMString_auto_ptr(
					gdome_str_mkref_dup((const gchar *)res->stringval)).get()));
		break;
	default: 
		throw Exception("parser.runtime",
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectBoolHandler(Pool& pool,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  Value *& result) {
	switch(res->type) {
	case XPATH_BOOLEAN: 
		result=new(pool) VBool(pool, res->boolval!=0);
		break;
	case XPATH_NODESET: 
		if(!(res->nodesetval && res->nodesetval->nodeNr))
			break;
		// else[nodeset] fall down to default
	default: 
		throw Exception("parser.runtime",
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectNumberHandler(Pool& pool,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  Value *& result) {
	switch(res->type) {
	case XPATH_NUMBER: 
		result=new(pool) VDouble(pool, res->floatval);
		break;
	case XPATH_NODESET:
		if(!(res->nodesetval && res->nodesetval->nodeNr))
			break;
		// else[nodeset] fall down to default
	default: 
		throw Exception("parser.runtime",
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectStringHandler(Pool& pool,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  Value *& result) {
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_STRING:
		result=new(pool) VString(
			pool.transcode(
				GdomeDOMString_auto_ptr(
					gdome_str_mkref_dup((const gchar *)res->stringval)).get()));
		break;
	case XPATH_NODESET: 
		if(!(res->nodesetval && res->nodesetval->nodeNr))
			break;
		// else[nodeset] fall down to default
	default: 
		throw Exception("parser.runtime",
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void _select(Request& r, const String& method_name, MethodParams *params) {
	_selectX(r, method_name, params,
		selectNodesHandler);
}

static void _selectSingle(Request& r, const String& method_name, MethodParams *params) {
	_selectX(r, method_name, params,
		selectNodeHandler);
}

static void _selectBool(Request& r, const String& method_name, MethodParams *params) {
	_selectX(r, method_name, params,
		selectBoolHandler);
}

static void _selectNumber(Request& r, const String& method_name, MethodParams *params) {
	_selectX(r, method_name, params,
		selectNumberHandler);
}

static void _selectString(Request& r, const String& method_name, MethodParams *params) {
	_selectX(r, method_name, params,
		selectStringHandler);
}

// constructor

MXnode::MXnode(Pool& apool, const char *aname, VStateless_class *abase) : 
	Methoded(apool, aname?aname:"xnode", abase), 

	consts(apool) 
{
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
	// NodeList getElementsByTagNameNS(in DOMString namespaceURI, in DOMString localName);
	add_native_method("getElementsByTagNameNS", Method::CT_DYNAMIC, _getElementsByTagNameNS, 2, 2);
	// void normalize();
	add_native_method("normalize", Method::CT_DYNAMIC, _normalize, 0, 0);

	/// parser
	// ^node.select[/some/xpath/query] = hash $.#[dnode]
	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 1);

	// ^node.selectSingle[/some/xpath/query] = first node [if any]
	add_native_method("selectSingle", Method::CT_DYNAMIC, _selectSingle, 1, 1);
	// ^node.selectBool[/some/xpath/query] = bool value [if any]
	add_native_method("selectBool", Method::CT_DYNAMIC, _selectBool, 1, 1);
	// ^node.selectNumber[/some/xpath/query] = double value [if any]
	add_native_method("selectNumber", Method::CT_DYNAMIC, _selectNumber, 1, 1);
	// ^node.selectString[/some/xpath/query] = strinv value [if any]
	add_native_method("selectString", Method::CT_DYNAMIC, _selectString, 1, 1);

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
