/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: xnode.C,v 1.16 2001/10/31 14:01:44 paf Exp $
*/
#include "classes.h"
#ifdef XML

#include "pa_request.h"
#include "pa_vxnode.h"

#include "xnode.h"

#include <util/XMLString.hpp>
#include <XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <XPath/XPathEvaluator.hpp>
#include <XPath/NodeRefList.hpp>
#include <XalanDOM/XalanElement.hpp>
#include <XalanDOM/XalanAttr.hpp>
#include <XalanDOM/XalanNodeList.hpp>

// defines

#define XNODE_CLASS_NAME "xnode"

// helpers

XalanNode& as_node(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	Value& value=params->as_no_junction(index, msg);
	if(strcmp(value.type(), VXNODE_TYPE)!=0)
		throw Exception(0, 0,
			&method_name,
			msg);

	VXnode& vnode=*static_cast<VXnode *>(&value);
	return vnode.get_node(pool, &method_name);
}

// helpers

XalanAttr& as_attr(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg) {
	XalanNode& node=as_node(pool, method_name, params, index, msg);

	if(node.getNodeType()!=XalanNode::ATTRIBUTE_NODE)
		throw Exception(0, 0, 
			&method_name,
			msg);

	return *reinterpret_cast<XalanAttr *>(&node);
}

// methods

// DOM1 node

// Node insertBefore(in Node newChild,in Node refChild) raises(DOMException);
static void _insertBefore(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& selfNode=vnode.get_node(pool, &method_name);

	XalanNode& newChild=as_node(pool, method_name, params, 0, "newChild must be node");
	XalanNode& refChild=as_node(pool, method_name, params, 1, "refChild must be node");
	
	try {
		if(XalanNode *retNode=selfNode.insertBefore(&newChild, &refChild)) {
			// write out result
			VXnode& result=*new(pool) VXnode(pool, retNode, false);
			r.write_no_lang(result);		
		}
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// Node replaceChild(in Node newChild,in Node oldChild) raises(DOMException);
static void _replaceChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& selfNode=vnode.get_node(pool, &method_name);

	XalanNode& newChild=as_node(pool, method_name, params, 0, "newChild must be node");
	XalanNode& refChild=as_node(pool, method_name, params, 1, "refChild must be node");
	
	try {
		if(XalanNode *retNode=selfNode.replaceChild(&newChild, &refChild)) {
			// write out result
			VXnode& result=*new(pool) VXnode(pool, retNode, false);
			r.write_no_lang(result);		
		}
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// Node removeChild(in Node oldChild) raises(DOMException);
static void _removeChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& selfNode=vnode.get_node(pool, &method_name);

	XalanNode& oldChild=as_node(pool, method_name, params, 0, "oldChild must be node");
	
	try {
		if(XalanNode *retNode=selfNode.removeChild(&oldChild)) {
			// write out result
			VXnode& result=*new(pool) VXnode(pool, retNode, false);
			r.write_no_lang(result);		
		}
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// Node appendChild(in Node newChild) raises(DOMException);
static void _appendChild(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& selfNode=vnode.get_node(pool, &method_name);

	XalanNode& newChild=as_node(pool, method_name, params, 0, "newChild must be node");
	
	try {
		if(XalanNode *retNode=selfNode.appendChild(&newChild)) {
			// write out result
			VXnode& result=*new(pool) VXnode(pool, retNode, false);
			r.write_no_lang(result);		
		}
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// boolean hasChildNodes();
static void _hasChildNodes(Request& r, const String& method_name, MethodParams *) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& node=vnode.get_node(pool, &method_name);

	// write out result
	VBool& result=*new(pool) VBool(pool, node.hasChildNodes());
	result.set_name(method_name);
	r.write_no_lang(result);
}

// Node cloneNode(in boolean deep);
/// @test ownerDocument=?
static void _cloneNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& node=vnode.get_node(pool, &method_name);

	bool deep=params->as_bool(0, "deep must be bool", r);

	// write out result
	VXnode& result=*new(pool) VXnode(pool, node.cloneNode(deep), true/*all sense goes here*/);
	result.set_name(method_name);
	r.write_no_lang(result);
}

// DOM1 element

XalanElement& get_self_element(Request& r, const String& method_name) {
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);
	XalanNode& node=vnode.get_node(pool, &method_name);

	if(node.getNodeType()!=XalanNode::ELEMENT_NODE)
		throw Exception(0, 0, 
			&method_name,
			"method can be called on node of ELEMENT type");

	return *reinterpret_cast<XalanElement *>(&node);
}

// DOMString getAttribute(in DOMString name);
static void _getAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");

	const XalanDOMString& attribute_value=element.getAttribute(*pool.transcode(name));
	// write out result
	r.write_no_lang(*new(pool) VString(pool.transcode(attribute_value)));
}

// void setAttribute(in DOMString name, in DOMString value) raises(DOMException);
static void _setAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");
	const String& attribute_value=params->as_string(1, "value must be string");

	try {
		element.setAttribute(
			*pool.transcode(name), 
			*pool.transcode(attribute_value));
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// void removeAttribute(in DOMString name) raises(DOMException);
static void _removeAttribute(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");

	try {
		element.removeAttribute(*pool.transcode(name));
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}

// Attr getAttributeNode(in DOMString name);
static void _getAttributeNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);
	const String& name=params->as_string(0, "name must be string");

	if(XalanAttr *attr=element.getAttributeNode(*pool.transcode(name))) {
		// write out result
		VXnode& result=*new(pool) VXnode(pool, attr, false);
		r.write_no_lang(result);
	}
}	

// Attr setAttributeNode(in Attr newAttr) raises(DOMException);
static void _setAttributeNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);
	XalanAttr& newAttr=as_attr(pool, method_name, params, 0, "newAttr must be ATTRIBUTE node");

	try {
		if(XalanAttr *returnAttr=element.setAttributeNode(&newAttr)) {
			// write out result
			VXnode& result=*new(pool) VXnode(pool, returnAttr, false);
			r.write_no_lang(result);
		}
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}	

// Attr removeAttributeNode(in Attr oldAttr) raises(DOMException);
static void _removeAttributeNode(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);
	XalanAttr& oldAttr=as_attr(pool, method_name, params, 0, "oldAttr must be ATTRIBUTE node");

	try {
		/*XalanAttr *returnAttr*/element.removeAttributeNode(&oldAttr);
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool, &method_name, e);
	}
}	

// NodeList getElementsByTagName(in DOMString name);
static void _getElementsByTagName(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	XalanElement& element=get_self_element(r, method_name);

	const String& name=params->as_string(0, "name must be string");

	VHash& result=*new(pool) VHash(pool);
	if(const XalanNodeList *nodes=
		element.getElementsByTagName(*pool.transcode(name))) {
		for(int i=0; i<nodes->getLength(); i++) {
			String& skey=*new(pool) String(pool);
			{
				char *buf=(char *)pool.malloc(MAX_NUMBER);
				snprintf(buf, MAX_NUMBER, "%d", i);
				skey << buf;
			}

			result.hash().put(skey, new(pool) VXnode(pool, nodes->item(i), false));
		}
	}

	// write out result
	r.write_no_lang(result);
}

// void normalize();
static void _normalize(Request& r, const String& method_name, MethodParams *) {
	XalanElement& element=get_self_element(r, method_name);

	element.normalize();
}


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

			result.hash().put(skey, new(pool) VXnode(pool, list.item(i), false));
		}
		result.set_name(method_name);
		r.write_no_lang(result);
	} catch(const XSLException& e) {
		Exception::provide_source(pool, &expression, e);
	}
}

static void _selectSingle(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must be string");
	const char *expression_cstr=expression.cstr();
	XalanDOMString dstring(expression_cstr);
	const XalanDOMChar *expression_dcstr=dstring.c_str();

	XPathEvaluator evaluator;
	// Initialize the XalanSourceTree subsystem...
//	XalanSourceTreeInit		theSourceTreeInit;
	// We'll use these to parse the XML file.
	XalanSourceTreeDOMSupport dom_support;

	try {
		 if(XalanNode *node=evaluator.selectSingleNode(dom_support, 
			&vnode.get_node(pool, &method_name), 
			expression_dcstr)) {

			VXnode& result=*new(pool) VXnode(pool, node, false);
			result.set_name(method_name);
			r.write_no_lang(result);
		}
	} catch(const XSLException& e) {
		Exception::provide_source(pool, &expression, e);
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
	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 1);

	// ^node.selectSingle[/some/xpath/query] = first dnode
	add_native_method("selectSingle", Method::CT_DYNAMIC, _selectSingle, 1, 1);

	// consts

#define CONST(name, value) \
	consts.put(*new(pool()) String(pool(), #name), new(pool()) VInt(pool(), value))

	CONST(ELEMENT_NODE, 1);
    CONST(ATTRIBUTE_NODE,  2);
    CONST(TEXT_NODE,  3);
    CONST(CDATA_SECTION_NODE,  4);
    CONST(ENTITY_REFERENCE_NODE,  5);
    CONST(ENTITY_NODE,  6);
    CONST(PROCESSING_INSTRUCTION_NODE,  7);
    CONST(COMMENT_NODE,  8);
    CONST(DOCUMENT_NODE,  9);
    CONST(DOCUMENT_TYPE_NODE,  10);
    CONST(DOCUMENT_FRAGMENT_NODE,  11);
    CONST(NOTATION_NODE,  12);

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
