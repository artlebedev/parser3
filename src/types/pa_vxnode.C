/** @node
	Parser: @b dnode parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vxnode.C,v 1.4 2001/10/15 14:13:22 parser Exp $
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

/*
void VXnode_cleanup(void *vxnode) {
	//_asm int 3;
	static_cast<VXnode *>(vxnode)->cleanup();
}
*/

Value *VXnode::get_element(const String& name) { 
	try {
		// $CLASS,$method
		if(Value *result=VStateless_object::get_element(name))
			return result;

		// fields

		XalanNode& node=get_node(pool(), &name);

		if(name=="nodeName") {
			return NEW VString(transcode(node.getNodeName()));
		} else if(name=="nodeValue") {
			return NEW VString(transcode(node.getNodeValue()));
		} else if(name=="nodeType") {
			return NEW VInt(pool(), node.getNodeType());
		} else if(name=="parentNode") {
			if(XalanNode *result_node=node.getParentNode())
				return NEW VXnode(pool(), result_node);
		} else if(name=="childNodes") {	
			if(const XalanNodeList *nodes=node.getChildNodes()) {
				VHash *result=NEW VHash(pool());
				for(int i=0; i<nodes->getLength(); i++) {
					String& skey=*NEW String(pool());
					{
						char *buf=(char *)malloc(MAX_NUMBER);
						snprintf(buf, MAX_NUMBER, "%d", i);
						skey << buf;
					}

					result->hash().put(skey, NEW VXnode(pool(), nodes->item(i)));
				}
				return result;
			}
		} else if(name=="firstChild") {
			if(XalanNode *result_node=node.getFirstChild())
				return NEW VXnode(pool(), result_node);
		} else if(name=="lastChild") {
			if(XalanNode *result_node=node.getLastChild())
				return NEW VXnode(pool(), result_node);
		} else if(name=="previousSibling") {
			if(XalanNode *result_node=node.getPreviousSibling())
				return NEW VXnode(pool(), result_node);
		} else if(name=="nextSibling") {
			if(XalanNode *result_node=node.getNextSibling())
				return NEW VXnode(pool(), result_node);
		} else if(name=="ownerDocument") {
			if(XalanDocument *document=node.getOwnerDocument())
				return NEW VXdoc(pool(), document);
		} else switch(node.getNodeType()) {
			case XalanNode::ELEMENT_NODE: 
				if(name=="attributes") {
					if(const XalanNamedNodeMap *attributes=node.getAttributes()) {
						VHash *result=NEW VHash(pool());
						for(int i=0; i<attributes->getLength(); i++) {
							XalanNode *attr_node=attributes->item(i);
							result->hash().put(
								transcode(attr_node->getNodeName()), 
								NEW VXnode(pool(), attr_node));
						}
						return result;
					}
				} else if(name=="tagName") {
					return NEW VString(transcode(static_cast<XalanElement *>(&node)->getTagName()));
				}
				break;
			case XalanNode::ATTRIBUTE_NODE: 
				if(name=="specified")
					return NEW VBool(pool(), static_cast<XalanAttr *>(&node)->getSpecified());
				break;
	/*
			case XalanNode::COMMENT_NODE: 
				substringData(unsigned int offset, unsigned int count)
	*/
			case XalanNode::PROCESSING_INSTRUCTION_NODE: 
				if(name=="target")
					return NEW VString(transcode(static_cast<XalanProcessingInstruction *>(&node)->getTarget()));
				break;
			case XalanNode::DOCUMENT_TYPE_NODE: 
				{
					XalanDocumentType& doctype=*static_cast<XalanDocumentType *>(&node);
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
					This function returns a named node map containing an entry for each notation declared in a document's DTD. More...
					*/
				}
				break;
			case XalanNode::NOTATION_NODE:
				{
					XalanNotation& notation=*static_cast<XalanNotation *>(&node);
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
		exception()._throw(pool(), &name, e);
	}

	return 0;
}

#endif
