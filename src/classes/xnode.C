/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001-2004 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "classes.h"
#ifdef XML

static const char * const IDENT_XNODE_C="$Date: 2004/02/11 15:33:13 $";

#include "pa_vmethod_frame.h"

#include "pa_charset.h"
#include "pa_request.h"
#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vvoid.h"
#include "pa_xml_exception.h"

#include "xnode.h"

extern "C" {
#include "gdomecore/gdome-xml-node.h"
#include "gdomecore/gdome-xml-document.h"
};
#include "gdome.h"
#include "libxml/xpath.h"

// global variable

DECLARE_CLASS_VAR(xnode, new MXnode, 0);

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

GdomeNode* as_node(MethodParams& params, 
		   int index, const char* msg) {
	GdomeNode* result;
	Value& value=params.as_no_junction(index, msg);
	if(Value* vxnode=value.as(VXNODE_TYPE, false))
		result=static_cast<VXnode*>(vxnode)->get_node();
	else {
		throw Exception("parser.runtime",
			0,
			msg);
	}

	return result;
}

// helpers

GdomeAttr* as_attr(MethodParams& params, 
		   int index, const char* msg) {
	GdomeNode* node=as_node(params, index, msg);
	GdomeException exc;
	if(gdome_n_nodeType(node, &exc)!=GDOME_ATTRIBUTE_NODE)
		throw Exception("parser.runtime",
			0,
			msg);

	return GDOME_A(node);
}

// methods

// DOM1 node

// Node insertBefore(in Node newChild,in Node refChild) raises(DOMException);
static void _insertBefore(Request& r, MethodParams& params) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* selfNode=vnode.get_node();
	GdomeNode* newChild=as_node(params, 0, "newChild must be node");
	GdomeNode* refChild=as_node(params, 1, "refChild must be node");
	
	GdomeException exc;
	if(GdomeNode* retNode=gdome_n_insertBefore(selfNode, newChild, refChild, &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, retNode));
	} else
		throw XmlException(0, exc);
}

// Node replaceChild(in Node newChild,in Node oldChild) raises(DOMException);
static void _replaceChild(Request& r, MethodParams& params) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* selfNode=vnode.get_node();
	GdomeNode* newChild=as_node(params, 0, "newChild must be node");
	GdomeNode* oldChild=as_node(params, 1, "oldChild must be node");
	
	GdomeException exc;
	if(GdomeNode* retNode=gdome_n_replaceChild(selfNode, newChild, oldChild, &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, retNode));
	} else
		throw XmlException(0, exc);
}

// Node removeChild(in Node oldChild) raises(DOMException);
static void _removeChild(Request& r, MethodParams& params) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* selfNode=vnode.get_node();
	GdomeNode* oldChild=as_node(params, 0, "oldChild must be node");
	
	GdomeException exc;
	if(GdomeNode* retNode=gdome_n_removeChild(selfNode, oldChild, &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, retNode));
	} else
		throw XmlException(0, exc);
}

// Node appendChild(in Node newChild) raises(DOMException);
static void _appendChild(Request& r, MethodParams& params) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* selfNode=vnode.get_node();
	GdomeNode* newChild=as_node(params, 0, "newChild must be node");
	
	GdomeException exc;
	if(GdomeNode* retNode=gdome_n_appendChild(selfNode, newChild, &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, retNode));		
	}  else
		throw XmlException(0, exc);
}

// boolean hasChildNodes();
static void _hasChildNodes(Request& r, MethodParams&) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* node=vnode.get_node();

	GdomeException exc;
	// write out result
	bool result=gdome_n_hasChildNodes(node, &exc)!=0;
	r.write_no_lang(*new VBool(result));
}

// Node cloneNode(in boolean deep);
static void _cloneNode(Request& r, MethodParams& params) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* node=vnode.get_node();

	bool deep=params.as_bool(0, "deep must be bool", r);

	GdomeException exc;
	// write out result
	r.write_no_lang(*new VXnode(&r.charsets, gdome_n_cloneNode(node, deep, &exc)));
}

// DOM1 element

GdomeElement* get_self_element(Request& r) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* node=vnode.get_node();

	GdomeException exc;
	if(gdome_n_nodeType(node, &exc)!=GDOME_ELEMENT_NODE)
		throw Exception("parser.runtime",
			0,
			"method can only be called on nodes of ELEMENT type");

	return GDOME_EL(node);
}



/// @bug attribute_value must be freed!  [// DOMString getAttribute(in DOMString name);
static void _getAttribute(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& name=params.as_string(0, "name must be string");

	GdomeException exc;
	GdomeDOMString *attribute_value=
		gdome_el_getAttribute(element, r.transcode(name).use(), &exc);
	// write out result
	r.write_no_lang(r.transcode(attribute_value));
}

// void setAttribute(in DOMString name, in DOMString value) raises(DOMException);
static void _setAttribute(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& name=params.as_string(0, "name must be string");
	const String& attribute_value=params.as_string(1, "value must be string");

	GdomeException exc;
	gdome_el_setAttribute(element,
		r.transcode(name).use(), 
		r.transcode(attribute_value).use(),
		&exc);
	if(exc)
		throw XmlException(0, exc);
}

// void removeAttribute(in DOMString name) raises(DOMException);
static void _removeAttribute(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& name=params.as_string(0, "name must be string");

	GdomeException exc;
	gdome_el_removeAttribute(element, r.transcode(name).use(), &exc);
	if(exc)
		throw XmlException(0, exc);
}

// Attr getAttributeNode(in DOMString name);
static void _getAttributeNode(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& name=params.as_string(0, "name must be string");

	GdomeException exc;
	if(GdomeAttr *attr=gdome_el_getAttributeNode(element, 
		r.transcode(name).use(), &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, (GdomeNode* )attr));
	} else if(exc)
		throw XmlException(0, exc);
}	

// Attr setAttributeNode(in Attr newAttr) raises(DOMException);
static void _setAttributeNode(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	GdomeAttr * newAttr=as_attr(params, 0, "newAttr must be ATTRIBUTE node");

	GdomeException exc;
	if(GdomeAttr *returnAttr=gdome_el_setAttributeNode(element, newAttr, &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, (GdomeNode* )returnAttr));
	} else
		throw XmlException(0, exc);
}	

// Attr removeAttributeNode(in Attr oldAttr) raises(DOMException);
static void _removeAttributeNode(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	GdomeAttr * oldAttr=as_attr(params, 0, "oldAttr must be ATTRIBUTE node");

	GdomeException exc;
	gdome_el_removeAttributeNode(element, oldAttr, &exc);
	if(exc)
		throw XmlException(0, exc);
}	

// NodeList getElementsByTagName(in DOMString name);
static void _getElementsByTagName(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);

	const String& name=params.as_string(0, "name must be string");

	VHash& result=*new VHash;
	GdomeException exc;
	if(GdomeNodeList *nodes=
		gdome_el_getElementsByTagName(element, r.transcode(name).use(), &exc)) {
		gulong length=gdome_nl_length(nodes, &exc);
		for(gulong i=0; i<length; i++)
			result.hash().put(
				String::Body::Format(i), 
				new VXnode(&r.charsets, gdome_nl_item(nodes, i, &exc)));
	} else if(exc)
		throw XmlException(0, exc);

	// write out result
	r.write_no_lang(result);
}

// DOM 2

// DOMString getAttributeNS(in DOMString namespaceURI, in DOMString localName);
static void _getAttributeNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& localName=params.as_string(0, "localName must be string");

	GdomeException exc;
	GdomeDOMString *attribute_value=
		gdome_el_getAttributeNS(element, 
			r.transcode(namespaceURI).use(),
			r.transcode(localName).use(), &exc);
	// write out result
	r.write_no_lang(r.transcode(attribute_value));
}

// void setAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName, in DOMString value) raises(DOMException);
static void _setAttributeNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& qualifiedName=params.as_string(1, "qualifiedName must be string");
	const String& attribute_value=params.as_string(2, "value must be string");

	GdomeException exc;
	gdome_el_setAttributeNS(element,
		r.transcode(namespaceURI).use(),
		r.transcode(qualifiedName).use(), 
		r.transcode(attribute_value).use(),
		&exc);
	if(exc)
		throw XmlException(0, exc);
}

// void removeAttributeNS(in DOMString namespaceURI, in DOMString localName) raises(DOMException);
static void _removeAttributeNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& localName=params.as_string(1, "localName must be string");

	GdomeException exc;
	gdome_el_removeAttributeNS(element, 
	r.transcode(namespaceURI).use(),
	r.transcode(localName).use(), 
	&exc);
	if(exc)
		throw XmlException(0, exc);
}

// Attr getAttributeNodeNS(in DOMString namespaceURI, in DOMString localName);
static void _getAttributeNodeNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& name=params.as_string(1, "name must be string");

	GdomeException exc;
	if(GdomeAttr *attr=gdome_el_getAttributeNodeNS(element, 
		r.transcode(namespaceURI).use(), r.transcode(name).use(), &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, (GdomeNode* )attr));
	} else if(exc)
		throw XmlException(0, exc);
}

// Attr setAttributeNodeNS(in Attr newAttr) raises(DOMException);
static void _setAttributeNodeNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);
	GdomeAttr * newAttr=as_attr(params, 0, "newAttr must be ATTRIBUTE node");

	GdomeException exc;
	if(GdomeAttr *returnAttr=gdome_el_setAttributeNodeNS(element, newAttr, &exc)) {
		// write out result
		r.write_no_lang(*new VXnode(&r.charsets, (GdomeNode* )returnAttr));
	} else
		throw XmlException(0, exc);
}

// boolean hasAttribute(in DOMString name) raises(DOMException);
static void _hasAttribute(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);

	const String& name=params.as_string(0, "name must be string");

	GdomeException exc;
	// write out result
	bool result=gdome_el_hasAttribute(element, 
	r.transcode(name).use(),
	&exc)!=0;
	r.write_no_lang(*new VBool(result));
}

// boolean hasAttributeNS(n DOMString namespaceURI, in DOMString localName) raises(DOMException);
static void _hasAttributeNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);

	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& localName=params.as_string(1, "localName must be string");

	GdomeException exc;
	// write out result
	bool result=gdome_el_hasAttributeNS(element, 
	r.transcode(namespaceURI).use(),
	r.transcode(localName).use(),
	&exc)!=0;
	r.write_no_lang(*new VBool(result));
}

static void _getElementsByTagNameNS(Request& r, MethodParams& params) {
	GdomeElement* element=get_self_element(r);

	// namespaceURI;localName
	const String& namespaceURI=params.as_string(0, "namespaceURI must be string");
	const String& localName=params.as_string(1, "localName must be string");

	GdomeException exc;
	VHash& result=*new VHash;
	if(GdomeNodeList *nodes=
		gdome_el_getElementsByTagNameNS(
			element, 
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

// void normalize();
static void _normalize(Request& r, MethodParams&) {
	VXnode& vnode=GET_SELF(r, VXnode);
	GdomeNode* selfNode=vnode.get_node();

	GdomeException exc;
	gdome_n_normalize(selfNode, &exc);
	if(exc)
		throw XmlException(0, exc);
}

static void _selectX(Request& r, MethodParams& params,
					 void (*handler)(Request& r,
							  const String& expression, 
							  xmlXPathObject_auto_ptr res,
							  Value*& result)) {
	VXnode& vnode=GET_SELF(r, VXnode);

	// expression
	const String& expression=params.as_string(0, "expression must be string");

	GdomeException exc;
	GdomeNode* dome_node=vnode.get_node();
	GdomeDocument *dome_document=gdome_n_ownerDocument(dome_node, &exc);
	if(!dome_document) // document does not own itself, so ownerDocument = 0
		dome_document=GDOME_DOC(dome_node); // and we need downcast
	xmlDoc *xml_document=gdome_xml_doc_get_xmlDoc(dome_document);
	xmlXPathContext_auto_ptr ctxt(xmlXPathNewContext(xml_document));
	ctxt->node=gdome_xml_n_get_xmlNode(dome_node);
	/*error to stderr for now*/
	xmlXPathObject_auto_ptr res(
		xmlXPathEvalExpression(BAD_CAST r.transcode(expression)->str, ctxt.get()));

	if(xmlHaveGenericErrors()) {
		GdomeException exc=0;
		throw XmlException(&expression, exc);
	}

	Value* result=0;
   	if(res.get())
		handler(r, expression, res, result);
	if(result)
		r.write_no_lang(*result);
}

static void selectNodesHandler(Request& r,
			       const String& expression,
			       xmlXPathObject_auto_ptr res,
			       Value*& result) {
	VHash& vhash=*new VHash;  result=&vhash;
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_NODESET:
		if(res->nodesetval)
			if(int size=res->nodesetval->nodeNr) {
				HashStringValue& hash=vhash.hash();
				for(int i=0; i<size; i++)
					hash.put(
						String::Body::Format(i), 
						new VXnode(
							&r.charsets, 
							gdome_xml_n_mkref(res->nodesetval->nodeTab[i])));
			}
		break;
	default: 
		throw Exception(0,
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectNodeHandler(Request& r,
			      const String& expression,
			      xmlXPathObject_auto_ptr res,
			      Value*& result) {
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_NODESET: 
		if(res->nodesetval && res->nodesetval->nodeNr) { // empty result strangly has NODESET  res->type
			if(res->nodesetval->nodeNr>1)
				throw Exception("parser.runtime",
					&expression,
					"resulted not in a single node (%d)", res->nodesetval->nodeNr);
			
			result=new VXnode(
				&r.charsets, 
				gdome_xml_n_mkref(res->nodesetval->nodeTab[0]));
		}
		break;
	case XPATH_BOOLEAN: 
		result=new VBool(res->boolval!=0);
		break;
	case XPATH_NUMBER: 
		result=new VDouble(res->floatval);
		break;
	case XPATH_STRING:
		result=new VString(r.transcode((xmlChar*)res->stringval));
		break;
	default: 
		throw Exception("parser.runtime",
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		// result=0;
		break; // never
	}
}

static void selectBoolHandler(Request&,
			      const String& expression,
			      xmlXPathObject_auto_ptr res,
			      Value*& result) {
	switch(res->type) {
	case XPATH_BOOLEAN: 
		result=new VBool(res->boolval!=0);
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

static void selectNumberHandler(Request&,
				const String& expression,
				xmlXPathObject_auto_ptr res,
				Value*& result) {
	switch(res->type) {
	case XPATH_NUMBER: 
		result=new VDouble(res->floatval);
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

static void selectStringHandler(Request& r,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  Value*& result) {
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_STRING:
		result=new VString(r.transcode((xmlChar*)res->stringval));
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

static void _select(Request& r, MethodParams& params) {
	_selectX(r, params,
		selectNodesHandler);
}

static void _selectSingle(Request& r, MethodParams& params) {
	_selectX(r, params,
		selectNodeHandler);
}

static void _selectBool(Request& r, MethodParams& params) {
	_selectX(r, params,
		selectBoolHandler);
}

static void _selectNumber(Request& r, MethodParams& params) {
	_selectX(r, params,
		selectNumberHandler);
}

static void _selectString(Request& r, MethodParams& params) {
	_selectX(r, params,
		selectStringHandler);
}

// constructor

/// @bug one can change const and ruin other's work, we need unchangable VIntConst class
MXnode::MXnode(const char* aname, VStateless_class *abase):
	Methoded(aname?aname:"xnode", abase)
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

	/// DOM2 element

	// DOMString getAttributeNS(in DOMString namespaceURI, in DOMString localName);
	add_native_method("getAttributeNS", Method::CT_DYNAMIC, _getAttributeNS, 2, 2);
	// void setAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName, in DOMString value) raises(DOMException);
	add_native_method("setAttributeNS", Method::CT_DYNAMIC, _setAttributeNS, 3, 3);
	// void removeAttributeNS(in DOMString namespaceURI, in DOMString localName) raises(DOMException);
	add_native_method("removeAttributeNS", Method::CT_DYNAMIC, _removeAttributeNS, 2, 2);
	// Attr getAttributeNodeNS(in DOMString namespaceURI, in DOMString localName);
	add_native_method("getAttributeNodeNS", Method::CT_DYNAMIC, _getAttributeNodeNS, 2, 2);
	// Attr setAttributeNodeNS(in Attr newAttr) raises(DOMException);
	add_native_method("setAttributeNodeNS", Method::CT_DYNAMIC, _setAttributeNodeNS, 1, 1);
	// boolean hasAttribute(in DOMString name) raises(DOMException);
	add_native_method("hasAttribute", Method::CT_DYNAMIC, _hasAttribute, 1, 1);
	// boolean hasAttributeNS(in DOMString namespaceURI, in DOMString localName) raises(DOMException);
	add_native_method("hasAttributeNS", Method::CT_DYNAMIC, _hasAttributeNS, 2, 2);

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
	consts.put(String::Body(#name), new VInt(GDOME_##name))

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

#else

// global variable

DECLARE_CLASS_VAR(xnode, 0, 0); // fictive

#endif
