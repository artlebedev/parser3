/** @node
	Parser: @b dnode parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vxnode.C,v 1.10 2001/11/05 10:21:28 paf Exp $
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vstring.h"
#include "pa_vbool.h"
#include "pa_vhash.h"

#include <util/XMLString.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <XalanDOM/XalanNamedNodeMap.hpp>
#include <XalanDOM/XalanAttr.hpp>
#include <XalanDOM/XalanProcessingInstruction.hpp>
#include <XalanDOM/XalanNodeList.hpp>
#include <XalanDOM/XalanElement.hpp>
#include <XalanDOM/XalanDocumentType.hpp>
#include <XalanDOM/XalanNotation.hpp>

void VXnode_cleanup(void *vxnode) {
	//_asm int 3;
	static_cast<VXnode *>(vxnode)->cleanup();
}

Value *VXnode::get_element(const String& name) { 
	try {
		// $CLASS,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;

		// fields

		XalanNode& selfNode=get_node(pool(), &name);

		if(name=="nodeName") {
			return NEW VString(transcode(selfNode.getNodeName()));
		} else if(name=="nodeValue") {
			return NEW VString(transcode(selfNode.getNodeValue()));
		} else if(name=="nodeType") {
			return NEW VInt(pool(), selfNode.getNodeType());
		} else if(name=="parentNode") {
			if(XalanNode *result_node=selfNode.getParentNode())
				return NEW VXnode(pool(), result_node, false);
		} else if(name=="childNodes") {	
			if(XalanNode *currentNode=selfNode.getFirstChild()) {
				VHash *result=NEW VHash(pool());
				int i=0;
				do {
					String& skey=*NEW String(pool());
					{
						char *buf=(char *)malloc(MAX_NUMBER);
						snprintf(buf, MAX_NUMBER, "%d", ++i);
						skey << buf;
					}
					result->hash(&name).put(skey, NEW VXnode(pool(), currentNode, false));
				} while(currentNode=currentNode->getNextSibling());
				return result;
			}
		} else if(name=="firstChild") {
			if(XalanNode *result_node=selfNode.getFirstChild())
				return NEW VXnode(pool(), result_node, false);
		} else if(name=="lastChild") {
			if(XalanNode *result_node=selfNode.getLastChild())
				return NEW VXnode(pool(), result_node, false);
		} else if(name=="previousSibling") {
			if(XalanNode *result_node=selfNode.getPreviousSibling())
				return NEW VXnode(pool(), result_node, false);
		} else if(name=="nextSibling") {
			if(XalanNode *result_node=selfNode.getNextSibling())
				return NEW VXnode(pool(), result_node, false);
		} else if(name=="ownerDocument") {
			if(XalanDocument *document=selfNode.getOwnerDocument())
				return NEW VXdoc(pool(), document, false/*owns not*/);
		} else switch(selfNode.getNodeType()) {
			case XalanNode::ELEMENT_NODE: 
				if(name=="attributes") {
					if(const XalanNamedNodeMap *attributes=selfNode.getAttributes()) {
						VHash *result=NEW VHash(pool());
						for(int i=0; i<attributes->getLength(); i++) {
							XalanNode *attr_node=attributes->item(i);
							result->hash(0).put(
								transcode(attr_node->getNodeName()), 
								NEW VXnode(pool(), attr_node, false));
						}
						return result;
					}
				} else if(name=="tagName") {
					return NEW VString(transcode(static_cast<XalanElement *>(&selfNode)->getTagName()));
				}
				break;
			case XalanNode::ATTRIBUTE_NODE: 
				if(name=="specified")
					return NEW VBool(pool(), static_cast<XalanAttr *>(&selfNode)->getSpecified());
				break;
	/*
			case XalanNode::COMMENT_NODE: 
				substringData(unsigned int offset, unsigned int count)
	*/
			case XalanNode::PROCESSING_INSTRUCTION_NODE: 
				if(name=="target")
					return NEW VString(transcode(static_cast<XalanProcessingInstruction *>(&selfNode)->getTarget()));
				break;
			case XalanNode::DOCUMENT_TYPE_NODE: 
				{
					XalanDocumentType& doctype=*static_cast<XalanDocumentType *>(&selfNode);
					if(name=="name") {
						// readonly attribute DOMString name;
						// The name of DTD; i.e., the name immediately following 
						// the DOCTYPE keyword in an XML source document.
						return NEW VString(transcode(doctype.getName()));
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
			case XalanNode::NOTATION_NODE:
				{
					XalanNotation& notation=*static_cast<XalanNotation *>(&selfNode);
					if(name=="publicId") {
						// readonly attribute DOMString publicId;
						return NEW VString(transcode(notation.getPublicId()));
					} else if(name=="systemId") {
						// readonly attribute DOMString systemId;
						return NEW VString(transcode(notation.getSystemId()));
					}
				}
				break;
		}
		
	} catch(const XalanDOMException& e)	{
		Exception::provide_source(pool(), &name, e);
	}

	return 0;
}

#endif
