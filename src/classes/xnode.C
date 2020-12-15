/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "classes.h"
#ifdef XML

#include "pa_vmethod_frame.h"

#include "pa_charset.h"
#include "pa_request.h"
#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vvoid.h"
#include "pa_xml_exception.h"
#include "pa_vbool.h"

#include "xnode.h"
#include "libxml/xpath.h"
#include "libxml/xpathInternals.h"

volatile const char * IDENT_XNODE_C="$Id: xnode.C,v 1.97 2020/12/15 17:10:29 moko Exp $" IDENT_XNODE_H;

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

xmlNode& as_node(MethodParams& params, int index, const char* msg) {
	Value& value=params.as_no_junction(index, msg);
	if(Value* vxnode=value.as(VXNODE_TYPE))
		return static_cast<VXnode*>(vxnode)->get_xmlnode();
	else
		throw Exception(PARSER_RUNTIME,
			0,
			msg);
}

xmlChar* as_xmlchar(Request& r, MethodParams& params, int index, const char* msg) {
	return r.transcode(params.as_string(index, msg));
}

xmlChar* as_xmlqname(Request& r, MethodParams& params, int index, const char* msg) {
	xmlChar* qname=r.transcode(params.as_string(index, msg ? msg : XML_QUALIFIED_NAME_MUST_BE_STRING));
	if(xmlValidateQName(qname, 0))
		throw XmlException(0, XML_INVALID_QUALIFIED_NAME, qname);
	return qname;
}

xmlChar* as_xmlncname(Request& r, MethodParams& params, int index, const char* msg) {
	xmlChar* ncname=r.transcode(params.as_string(index, msg ? msg : XML_NC_NAME_MUST_BE_STRING));
	if(xmlValidateNCName(ncname, 0))
		throw XmlException(0, XML_INVALID_NC_NAME, ncname);
	return ncname;
}

xmlChar* as_xmlname(Request& r, MethodParams& params, int index, const char* msg) {
	xmlChar* localName=r.transcode(params.as_string(index, msg ? msg : XML_LOCAL_NAME_MUST_BE_STRING));
	if(xmlValidateName(localName, 0))
		throw XmlException(0, XML_INVALID_LOCAL_NAME, localName);
	return localName;
}

xmlChar* as_xmlnsuri(Request& r, MethodParams& params, int index) {
	return r.transcode(params.as_string(index, XML_NAMESPACEURI_MUST_BE_STRING));
}

xmlAttr& as_attr(MethodParams& params, int index, const char* msg) {
	xmlNode& xmlnode=as_node(params, index, msg);
	if(xmlnode.type!=XML_ATTRIBUTE_NODE)
		throw Exception(PARSER_RUNTIME,
			0,
			msg);

	return *(xmlAttr*)&xmlnode;
}

static void writeNode(Request& r, VXdoc& xdoc, xmlNode* node) {
	if(!node|| xmlHaveGenericErrors())
		throw XmlException(0, r); // OOM, bad name, things like that

	// write out result
	r.write(xdoc.wrap(*node));
}

static xmlNode* pa_getAttributeNodeNS(xmlNode& selfNode, 
									  const xmlChar* localName,
									  const xmlChar* namespaceURI) 
{
	for(xmlNode* currentNode=(xmlNode*)selfNode.properties;
		currentNode;
		currentNode=currentNode->next)
	{
		if(!namespaceURI || (currentNode->ns && xmlStrEqual(currentNode->ns->href, namespaceURI)) )
			if(!localName || xmlStrEqual(currentNode->name, localName))
				return currentNode;
	}
	return 0;
}

xmlNs& pa_xmlMapNs(xmlDoc& doc, const xmlChar *href, const xmlChar *prefix) {
	assert(href);
	// prefix can be null

	xmlNs *cur=doc.oldNs;
	while (cur != NULL &&
				 ((cur->prefix == NULL && prefix != NULL) ||
					(cur->prefix != NULL && prefix == NULL) ||
					!xmlStrEqual (cur->prefix, prefix)) &&
				 !xmlStrEqual (cur->href, href))
		cur = cur->next;

	if (cur == NULL) {
		cur = xmlNewNs (NULL, href, prefix);
		if(!cur || xmlHaveGenericErrors())
			throw XmlException();
		cur->next = doc.oldNs;
		doc.oldNs = cur;
	}

	return *cur;
}

/// @todo: проверить, обновляется ли parent!
static void pa_addAttributeNode(xmlNode& selfNode, xmlAttr& attrNode) 
{
	if(attrNode.type!=XML_ATTRIBUTE_NODE)
		throw Exception(PARSER_RUNTIME,
			0,
			"must be ATTRIBUTE_NODE");

    /*
     * Add it at the end to preserve parsing order ...
     */
    if (selfNode.properties == NULL) {
        selfNode.properties = &attrNode;
    } else {
        xmlAttrPtr prev = selfNode.properties;

        while (prev->next != NULL)
            prev = prev->next;
        prev->next = &attrNode;
        attrNode.prev = prev;
    }

    if (xmlIsID(selfNode.doc, &selfNode, &attrNode) == 1)
        xmlAddID(NULL, selfNode.doc, xmlNodeGetContent((xmlNode*)&attrNode), &attrNode);
}

static const xmlChar *
pa_xmlGetNsURI(xmlNode *node) {
	if (node == NULL || node->ns == NULL)
		return NULL;

	return node->ns->href;
}

#ifndef DOXYGEN
struct AccumulateFoundInfo
{
	HashStringValue* hash;
	VXdoc* vdoc;
	int index;
};
#endif
static void AccumulateFound(xmlNode& node, AccumulateFoundInfo* info)
{
	info->hash->put(
		String::Body::Format(info->index++), 
		&info->vdoc->wrap(node));
}
template<typename I> static void
pa_xmlNamedPreorderTraversal (
							  xmlNode *root, 
							  xmlChar *tagURI, 
							  xmlChar *tagName, 
							  void callback(xmlNode& node, I info),
							  I info) 
{
	for(xmlNode *iter=root->children; iter; iter = iter->next) {
		if(iter->type == XML_ELEMENT_NODE &&
			(xmlStrEqual(iter->name, tagName) ||
			xmlStrEqual(tagName, (const xmlChar*)"*"))) {
				if(tagURI != NULL &&
					(xmlStrEqual(pa_xmlGetNsURI(iter), tagURI) ||
					xmlStrEqual(tagURI, (const xmlChar*)"*")))
					callback(*iter, info);
				else if(tagURI == NULL)
					callback(*iter, info);
			}
			pa_xmlNamedPreorderTraversal(iter, tagURI, tagName, callback, info);
	}

	return;
}


// methods

// DOM1 node

// Node insertBefore(in Node newChild,in Node refChild) raises(DOMException);
static void _insertBefore(Request& r, MethodParams& params) {
	xmlNode& newChild=as_node(params, 0, "newChild must be node");
	xmlNode& refChild=as_node(params, 1, "refChild must be node");

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();

	//xmlNode& selfNode=vnode.get_xmlnode();
	
	xmlNode* retNode=xmlAddPrevSibling(&refChild, &newChild);
	// write out result
	writeNode(r, vxdoc, retNode);
}

// Node replaceChild(in Node newChild,in Node oldChild) raises(DOMException);
static void _replaceChild(Request& r, MethodParams& params) {
	xmlNode& newChild=as_node(params, 0, "newChild must be node");
	xmlNode& oldChild=as_node(params, 1, "oldChild must be node");

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vxdoc.get_xmldoc();
	xmlNode& selfNode=vnode.get_xmlnode();

	if(newChild.doc!=&xmldoc)
		throw Exception("xml.dom",
			0,
			"WRONG_DOCUMENT_ERR");
	if(oldChild.doc!=&xmldoc)
		throw Exception("xml.dom",
			0,
			"WRONG_DOCUMENT_ERR");

	if(oldChild.parent!=&selfNode)
		throw Exception("xml.dom",
			0,
			"NOT_FOUND_ERR");

	xmlNode* refChild=oldChild.next;
	xmlUnlinkNode(&oldChild);
	xmlNode* retNode;
	if(refChild)
		retNode=xmlAddPrevSibling(refChild, &newChild);
	else
		retNode=xmlAddChild(&selfNode, &newChild);

	// write out result
	writeNode(r, vxdoc, retNode);
}

// Node removeChild(in Node oldChild) raises(DOMException);
static void _removeChild(Request& r, MethodParams& params) {
	xmlNode& oldChild=as_node(params, 0, "refChild must be node");

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vxdoc.get_xmldoc();
//	xmlNode& selfNode=vnode.get_xmlnode();
	
	if(oldChild.doc!=&xmldoc)
		throw Exception("xml.dom",
			0,
			"WRONG_DOCUMENT_ERR");

	xmlUnlinkNode(&oldChild);
	// write out result
	writeNode(r, vxdoc, &oldChild);
}

// Node appendChild(in Node newChild) raises(DOMException);
static void _appendChild(Request& r, MethodParams& params) {
	xmlNode& newChild=as_node(params, 0, "newChild must be node");

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlNode& selfNode=vnode.get_xmlnode();
	
	xmlNode* retNode=xmlAddChild(&selfNode, &newChild);
	// write out result
	writeNode(r, vxdoc, retNode);
}

// boolean hasChildNodes();
static void _hasChildNodes(Request& r, MethodParams&) {
	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& node=vnode.get_xmlnode();

	// write out result
	r.write(VBool::get(node.children!=0));
}

// Node cloneNode(in boolean deep);
static void _cloneNode(Request& r, MethodParams& params) {
	bool deep=params.as_bool(0, "deep must be bool", r);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& selfNode=vnode.get_xmlnode();
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vxdoc.get_xmldoc();

	xmlNode* retNode=xmlDocCopyNode(&selfNode, &xmldoc, deep?1: 0);
	// write out result
	writeNode(r, vxdoc, retNode);
}

// DOM1 element

xmlNode& get_self_element(VXnode& vnode) {
	xmlNode& node=vnode.get_xmlnode();

	if(node.type!=XML_ELEMENT_NODE)
		throw Exception(PARSER_RUNTIME,
			0,
			"method can only be called on nodes of ELEMENT type");

	return node;
}

// DOMString getAttribute(in DOMString name);
static void _getAttribute(Request& r, MethodParams& params) {
	const xmlChar* name=as_xmlname(r, params, 0);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);

	// @todo: when name="xmlns"
	xmlChar* attribute_value=xmlGetProp(&element, name);
	// write out result
	r.write(r.transcode(attribute_value));
}

// void setAttribute(in DOMString name, in DOMString value) raises(DOMException);
static void _setAttribute(Request& r, MethodParams& params) {
	const xmlChar* name=as_xmlname(r, params, 0);
	const xmlChar* attribute_value=as_xmlchar(r, params, 1, XML_VALUE_MUST_BE_STRING);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);

	// @todo: when name="xmlns"
	if(!xmlSetProp(&element, name,  attribute_value))
		throw XmlException(0, r);
}

// void removeAttribute(in DOMString name) raises(DOMException);
static void _removeAttribute(Request& r, MethodParams& params) {
	const xmlChar* name=as_xmlname(r, params, 0);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);

	// @todo: when name="xmlns"
	xmlUnsetProp(&element, name);
}

// Attr getAttributeNode(in DOMString name);
static void _getAttributeNode(Request& r, MethodParams& params) {
	const xmlChar* localName=as_xmlname(r, params, 0);

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlNode& element=get_self_element(vnode);

	if(xmlNode* retNode=pa_getAttributeNodeNS(element, localName, 0)){
		// write out result
		writeNode(r, vxdoc, retNode);
	}
}	

// Attr setAttributeNode(in Attr newAttr) raises(DOMException);
// Attr setAttributeNodeNS(in Attr newAttr) raises(DOMException);
static void _setAttributeNode(Request& r, MethodParams& params) {
	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlNode& element=get_self_element(vnode);
	xmlDoc& xmldoc=vxdoc.get_xmldoc();

	xmlAttr& newAttr=as_attr(params, 0, "newAttr must be ATTRIBUTE node");

	if(newAttr.doc!=&xmldoc)
		throw Exception("xml.dom",
			0,
			"WRONG_DOCUMENT_ERR");

	if(newAttr.parent)
		throw Exception("xml.dom",
			0,
			"INUSE_ATTRIBUTE_ERR");
	
	if(xmlNode* retNode=pa_getAttributeNodeNS(element, newAttr.name, pa_xmlGetNsURI((xmlNode*)&newAttr))) {
		// write out result
		writeNode(r, vxdoc, retNode);
		xmlUnlinkNode(retNode);
	}

	pa_addAttributeNode(element, newAttr);
}	

// Attr removeAttributeNode(in Attr oldAttr) raises(DOMException);
static void _removeAttributeNode(Request& r, MethodParams& params) {
	xmlAttr& oldAttr=as_attr(params, 0, "oldAttr must be ATTRIBUTE node");

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlNode& element=get_self_element(vnode);

	if(oldAttr.parent!=&element)
		throw Exception("xml.dom",
			0,
			"NOT_FOUND_ERR");

	xmlUnlinkNode((xmlNode*)&oldAttr);

	// write out result
	writeNode(r, vxdoc, (xmlNode*)&oldAttr);
}	

// NodeList getElementsByTagName(in DOMString name);
// '*' means all tags
static void _getElementsByTagName(Request& r, MethodParams& params) {
	xmlChar* tagName=as_xmlchar(r, params, 0, XML_LOCAL_NAME_MUST_BE_STRING);
	if(xmlValidateName(tagName, 0) != 0 && strcmp((const char*)tagName, "*") != 0)
		throw XmlException(0, XML_INVALID_LOCAL_NAME, tagName);

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlNode& xmlnode=vnode.get_xmlnode();

	VHash& result=*new VHash;
	AccumulateFoundInfo info={&result.hash(), &vxdoc, 0};
	pa_xmlNamedPreorderTraversal(&xmlnode, 
							  0, 
							  tagName, 
							  AccumulateFound,
							  &info);

	// write out result
	r.write(result);
}

// DOM 2

// DOMString getAttributeNS(in DOMString namespaceURI, in DOMString localName);
static void _getAttributeNS(Request& r, MethodParams& params) {
	xmlChar* namespaceURI=as_xmlnsuri(r, params, 0);
	xmlChar* localName=as_xmlname(r, params, 1);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);
	
	// todo: when name="xmlns"
	xmlChar* attribute_value=xmlGetNsProp(&element, localName, namespaceURI);
	// write out result
	r.write(r.transcode(attribute_value));
}


// void setAttributeNS(in DOMString namespaceURI, in DOMString qualifiedName, in DOMString value) raises(DOMException);
static void _setAttributeNS(Request& r, MethodParams& params) {
	const xmlChar* namespaceURI=as_xmlnsuri(r, params, 0);
	const xmlChar* qualifiedName=as_xmlqname(r, params, 1);
	const xmlChar* attribute_value=as_xmlchar(r, params, 2, XML_VALUE_MUST_BE_STRING);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vxdoc.get_xmldoc();

	xmlChar* prefix=0;
	xmlChar* localName=xmlSplitQName2(qualifiedName, &prefix);

	// @todo: name=xmlns
	xmlAttr* attrNode;
	if(localName) {
		xmlNs& ns=pa_xmlMapNs(xmldoc, namespaceURI, prefix);
		
		attrNode=xmlSetNsProp(&element, &ns,
			localName,
			attribute_value);
	} else {
		attrNode=xmlSetProp(&element, 
			qualifiedName/*unqualified, actually*/,
			attribute_value);
	}

	if(!attrNode)
		throw XmlException(0, r);
}

// void removeAttributeNS(in DOMString namespaceURI, in DOMString localName) raises(DOMException);
static void _removeAttributeNS(Request& r, MethodParams& params) {
	const xmlChar* namespaceURI=as_xmlnsuri(r, params, 0);
	const xmlChar* localName=as_xmlname(r, params, 1);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vxdoc.get_xmldoc();

	// @todo: when name="xmlns"
	xmlNs& ns=pa_xmlMapNs(xmldoc, namespaceURI, 0);
	xmlUnsetNsProp(&element, &ns, localName);
}

// Attr getAttributeNodeNS(in DOMString namespaceURI, in DOMString localName);
static void _getAttributeNodeNS(Request& r, MethodParams& params) {
	const xmlChar* namespaceURI=as_xmlnsuri(r, params, 0);
	const xmlChar* localName=as_xmlname(r, params, 1);

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vxdoc=vnode.get_vxdoc();
	xmlNode& element=get_self_element(vnode);

	if(xmlNode* retNode=pa_getAttributeNodeNS(element, localName, namespaceURI)){
		// write out result
		writeNode(r, vxdoc, retNode);
	}
}	

// boolean hasAttribute(in DOMString name) raises(DOMException);
static void _hasAttribute(Request& r, MethodParams& params) {
	const xmlChar* name=as_xmlname(r, params, 0);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);

	// @todo: when name="xmlns"
	// write out result
	r.write(VBool::get(xmlHasProp(&element, name)!=0));
}

// boolean hasAttributeNS(n DOMString namespaceURI, in DOMString localName) raises(DOMException);
static void _hasAttributeNS(Request& r, MethodParams& params) {
	const xmlChar* namespaceURI=as_xmlnsuri(r, params, 0);
	const xmlChar* localName=as_xmlname(r, params, 1);

	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);

	// write out result
	r.write(VBool::get(xmlHasNsProp(&element, localName, namespaceURI)!=0));
}

// boolean hasAttributes
static void _hasAttributes(Request& r, MethodParams&) {
	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& element=get_self_element(vnode);

	// write out result
	r.write(VBool::get(element.properties!=0));
}

// NodeList getElementsByTagNameNS(in DOMString namespaceURI, in DOMString localName);
// '*' as namespaceURI means get all namespaces
// '*' as localName means get all tags
static void _getElementsByTagNameNS(Request& r, MethodParams& params) {
	xmlChar* namespaceURI=as_xmlchar(r, params, 0, XML_NAMESPACEURI_MUST_BE_STRING);

	xmlChar* localName=as_xmlchar(r, params, 1, XML_LOCAL_NAME_MUST_BE_STRING);
	if(xmlValidateName(localName, 0) != 0 && strcmp((const char*)localName, "*") != 0)
		throw XmlException(0, XML_INVALID_LOCAL_NAME, localName);

	VXnode& vnode=GET_SELF(r, VXnode);
	VXdoc& vdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	VHash& result=*new VHash;
	AccumulateFoundInfo info={&result.hash(), &vdoc, 0};
	pa_xmlNamedPreorderTraversal((xmlNode*)&xmldoc, 
							  namespaceURI, 
							  localName, 
							  AccumulateFound,
							  &info);

	// write out result
	r.write(result);
}


// void normalize();
static void _normalize(Request&, MethodParams&) {
/*maybe someday
	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& selfNode=vnode.get_xmlnode();

	gdome_n_normalize(selfNode, &exc);
	if(exc)
		throw XmlException(0, exc);
*/
}

#ifndef DOXYGEN
struct Register_one_ns_info {
	Request* r;
	xmlXPathContextPtr ctxt;
};
#endif
static void register_one_ns(
								  HashStringValue::key_type key, 
								  HashStringValue::value_type value, 
								  Register_one_ns_info* info) {
	if(const String* svalue=value->get_string())
		xmlXPathRegisterNs(info->ctxt, 
			info->r->transcode(key), 
			info->r->transcode(*svalue));
	else
		throw Exception(PARSER_RUNTIME,
			new String(key, String::L_TAINTED),
			"value is %s, must be string or number", value->type());
}
static void _selectX(Request& r, MethodParams& params,
					 void (*handler)(Request& r,
							  const String& expression, 
							  xmlXPathObject_auto_ptr res,
							  VXdoc& xdoc,
							  Value*& result)) 
{
	VXnode& vnode=GET_SELF(r, VXnode);
	xmlNode& xmlnode=vnode.get_xmlnode();
	VXdoc& vdoc=vnode.get_vxdoc();
	xmlDoc& xmldoc=vdoc.get_xmldoc();

	// expression
	const String& expression=params.as_string(0, "expression must be string");
	xmlXPathContext_auto_ptr ctxt(xmlXPathNewContext(&xmldoc));
	{
		Register_one_ns_info info={&r, ctxt.get()};
		vdoc.search_namespaces.hash().for_each<Register_one_ns_info*>(register_one_ns, &info);
	}
	ctxt->node=&xmlnode;
	/*error to stderr for now*/
	xmlXPathObject_auto_ptr res(
		xmlXPathEvalExpression(r.transcode(expression), ctxt.get()));

	if(xmlHaveGenericErrors())
		throw XmlException(0, r);

	Value* result=0;
   	if(res.get())
		handler(r, expression, res, vdoc, result);
	if(result)
		r.write(*result);
}

static void selectNodesHandler(Request&,
			       const String& expression,
			       xmlXPathObject_auto_ptr res,
				   VXdoc& xdoc,
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
						&xdoc.wrap(*res->nodesetval->nodeTab[i]));
			}
		break;
	default: 
		throw Exception(PARSER_RUNTIME,
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectNodeHandler(Request& r,
			      const String& expression,
				  xmlXPathObject_auto_ptr res,
				  VXdoc& xdoc,
				  Value*& result) {
	switch(res->type) {
	case XPATH_UNDEFINED: 
		break;
	case XPATH_NODESET: 
		if(res->nodesetval && res->nodesetval->nodeNr) { // empty result strangely has NODESET  res->type
			if(res->nodesetval->nodeNr>1)
				throw Exception(PARSER_RUNTIME,
					&expression,
					"resulted not in a single node (%d)", res->nodesetval->nodeNr);
			
			result=&xdoc.wrap(*res->nodesetval->nodeTab[0]);
		}
		break;
	case XPATH_BOOLEAN: 
		result=&VBool::get(res->boolval!=0);
		break;
	case XPATH_NUMBER: 
		result=new VDouble(res->floatval);
		break;
	case XPATH_STRING:
		result=new VString(r.transcode((xmlChar*)res->stringval));
		break;
	default: 
		throw Exception(PARSER_RUNTIME,
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
	}
}

static void selectBoolHandler(Request&,
			      const String& expression,
			      xmlXPathObject_auto_ptr res,
				  VXdoc& /*xdoc*/,
			      Value*& result) {
	switch(res->type) {
	case XPATH_BOOLEAN: 
		result=&VBool::get(res->boolval!=0);
		break;
	case XPATH_NODESET: 
		if(!(res->nodesetval && res->nodesetval->nodeNr))
			break;
		// else[nodeset] fall down to default
	default: 
		throw Exception(PARSER_RUNTIME,
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectNumberHandler(Request&,
				const String& expression,
				xmlXPathObject_auto_ptr res,
				VXdoc& /*xdoc*/,
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
		throw Exception(PARSER_RUNTIME,
			&expression,
			"wrong xmlXPathEvalExpression result type (%d)", res->type);
		break; // never
	}
}

static void selectStringHandler(Request& r,
							  const String& expression,
							  xmlXPathObject_auto_ptr res,
							  VXdoc& /*xdoc*/,
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
		throw Exception(PARSER_RUNTIME,
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
MXnode::MXnode(const char* aname): Methoded(aname?aname:"xnode") {
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
	add_native_method("setAttributeNodeNS", Method::CT_DYNAMIC, _setAttributeNode, 1, 1);
	// boolean hasAttribute(in DOMString name) raises(DOMException);
	add_native_method("hasAttribute", Method::CT_DYNAMIC, _hasAttribute, 1, 1);
	// boolean hasAttributeNS(in DOMString namespaceURI, in DOMString localName) raises(DOMException);
	add_native_method("hasAttributeNS", Method::CT_DYNAMIC, _hasAttributeNS, 2, 2);
	// boolean hasAttributes
	add_native_method("hasAttributes", Method::CT_DYNAMIC, _hasAttributes, 0, 0);
	

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

#define CONST(name) consts.put(#name, new VInt(XML_##name))
#define CONST2(name, value) consts.put(#name, new VInt(value))

	CONST(ELEMENT_NODE);
	CONST(ATTRIBUTE_NODE);
	CONST(TEXT_NODE);
	CONST(CDATA_SECTION_NODE);
	CONST2(ENTITY_REFERENCE_NODE, XML_ENTITY_REF_NODE);
	CONST(ENTITY_NODE);
	CONST2(PROCESSING_INSTRUCTION_NODE, XML_PI_NODE);
	CONST(COMMENT_NODE);
	CONST(DOCUMENT_NODE);
	CONST(DOCUMENT_TYPE_NODE);
	CONST2(DOCUMENT_FRAGMENT_NODE, XML_DOCUMENT_FRAG_NODE);
	CONST(NOTATION_NODE);
}

#endif
