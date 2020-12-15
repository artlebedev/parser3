/** @node
	Parser: @b dnode parser type.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vstring.h"
#include "pa_vbool.h"
#include "pa_vhash.h"
#include "pa_request_charsets.h"
#include "pa_charset.h"
#include "pa_xml_exception.h"

volatile const char * IDENT_PA_VXNODE_C="$Id: pa_vxnode.C,v 1.60 2020/12/15 17:10:44 moko Exp $" IDENT_PA_VXNODE_H;

Request_charsets& VXnode::charsets() { 
	return get_vxdoc().charsets();
}

/// VXnode: true	 
Value& VXnode::as_expr_result() { return VBool::get(as_bool()); }


Value* VXnode::get_element(const String& aname) { 
	// $method
	if(Value* result=VStateless_object::get_element(aname))
		return result;

	// fields
	xmlNode& selfNode=get_xmlnode();

	if(aname=="nodeName") {
		return new VString(charsets().source().transcode(selfNode.name));
	} else if(aname=="nodeValue") {
		switch(selfNode.type) {
		case XML_ATTRIBUTE_NODE:
		case XML_PI_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_COMMENT_NODE:
		case XML_TEXT_NODE:
			return new VString(charsets().source().transcode(xmlNodeGetContent(&selfNode)));
		default:
			return 0;
		}
	} else if(aname=="nodeType") {
		return new VInt(selfNode.type);
	} else if(aname=="parentNode") {
		if(xmlNode* result_node=selfNode.parent)
			return &get_vxdoc().wrap(*result_node);
		return 0;
	} else if(aname=="childNodes") {	
		if(xmlNode* currentNode=selfNode.children) {
			VHash* result=new VHash;
			int i=0;
			do {
				result->hash().put(
					String::Body::Format(i++), 
					&get_vxdoc().wrap(*currentNode));
			} while((currentNode=currentNode->next));
			return result;
		}
		return 0;
	} else if(aname=="firstChild") {
		if(xmlNode* result_node=selfNode.children)
			return &get_vxdoc().wrap(*result_node);
		return 0;
	} else if(aname=="lastChild") {
		if(xmlNode* result_node=selfNode.last)
			return &get_vxdoc().wrap(*result_node);
		return 0;
	} else if(aname=="previousSibling") {
		if(xmlNode* result_node=selfNode.prev)
			return &get_vxdoc().wrap(*result_node);
		return 0;
	} else if(aname=="nextSibling") {
		if(xmlNode* result_node=selfNode.next)
			return &get_vxdoc().wrap(*result_node);
		return 0;
	} else if(aname=="ownerDocument") {
		return &get_vxdoc();
	} else switch(selfNode.type) {
		case XML_ELEMENT_NODE: 
			if(aname=="attributes") {
				if(xmlNode* currentNode=(xmlNode*)selfNode.properties) {
					VHash* result=new VHash;
					do {
						result->hash().put(
							charsets().source().transcode(currentNode->name),
							&get_vxdoc().wrap(*currentNode));
					} while((currentNode=currentNode->next));
					return result;
				}
				return 0;
			} else if(aname=="tagName")
				return new VString(charsets().source().transcode(selfNode.name));
			else if(aname=="prefix")
				return (selfNode.ns ? new VString(charsets().source().transcode(selfNode.ns->prefix)) : 0);
			else if(aname=="namespaceURI")
				return (selfNode.ns ? new VString(charsets().source().transcode(selfNode.ns->href)) : 0);
			break;
		case XML_ATTRIBUTE_NODE: 
			if(aname=="specified")
				return &VBool::get(true); // were not working in gdome, leaving out
			else if(aname=="name")
				return new VString(charsets().source().transcode(selfNode.name));
			else if(aname=="value")
				return new VString(charsets().source().transcode(xmlNodeGetContent(&selfNode)));
			else if(aname=="prefix")
				return (selfNode.ns ? new VString(charsets().source().transcode(selfNode.ns->prefix)) : 0);
			else if(aname=="namespaceURI")
				return (selfNode.ns ? new VString(charsets().source().transcode(selfNode.ns->href)) : 0);
			break;
/*
		case XML_COMMENT_NODE: 
			substringData(unsigned int offset, unsigned int count)
*/
		case XML_PI_NODE: 
			if(aname=="target")
				return new VString(charsets().source().transcode(selfNode.name));
			else if(aname=="data")
				return new VString(charsets().source().transcode(xmlNodeGetContent(&selfNode)));
			break;
		case XML_DTD_NODE: 
			{
				if(aname=="name") {
					// readonly attribute DOMString aname;
					// The aname of DTD; i.e., the aname immediately following 
					// the DOCTYPE keyword in an XML source document.
					return new VString(charsets().source().transcode(selfNode.name));
				}
				/*
				readonly attribute NamedNodeMap entities;
				readonly attribute NamedNodeMap notations;
				virtual const XalanNamedNodeMap* getEntities () const = 0 
				This function returns a NamedNodeMap containing the general entities, both external and internal, declared in the DTD. More...
				virtual const XalanNamedNodeMap* getNotations () const = 0 
				This function returns a named selfNode map containing an entry for each notation declared in a document's DTD. More...
				*/
			}
			break;
			/* someday
		case XML_NOTATION_NODE:
			{
				GdomeNotation *notation=XML_NOT(selfNode);
				if(aname=="publicId") {
					// readonly attribute DOMString publicId;
					return new VString(charsets().source().transcode(gdome_not_publicId(notation, &exc)));
				} else if(aname=="systemId") {
					// readonly attribute DOMString systemId;
					return new VString(charsets().source().transcode(gdome_not_systemId(notation, &exc)));
				}
			}
			break;
			*/
		default:
			// calm compiler on unhandled cases
			break;
	}
	
	return bark("%s field not found", &aname);
}

const VJunction* VXnode::put_element(const String& aname, Value* avalue)
{ 
	xmlNode& selfNode=get_xmlnode();

	if(aname=="nodeValue") {
		xmlNodeSetContent(&selfNode, charsets().source().transcode(avalue->as_string().cstr_to_string_body_untaint(String::L_XML, 0, &charsets() )));
		return 0;
	}

	return Value::put_element(aname, avalue);
}
#endif
