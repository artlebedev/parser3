/** @node
	Parser: @b dnode parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)
*/
#include "pa_config_includes.h"
#ifdef XML
static const char *RCSId="$Id: pa_vdnode.C,v 1.6 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_vdnode.h"
#include "pa_vdom.h"
#include "pa_vstring.h"
#include "pa_vbool.h"

#include <util/XMLString.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <XalanDOM/XalanNamedNodeMap.hpp>
#include <XalanDOM/XalanAttr.hpp>
#include <XalanDOM/XalanProcessingInstruction.hpp>

/*
void VDnode_cleanup(void *vnode) {
	//_asm int 3;
	static_cast<VDDom *>(vnode)->cleanup();
}
*/

/// VDnode: $CLASS,$method
Value *VDnode::get_element(const String& aname) { 
	// $CLASS,$method
	if(Value *result=VStateless_object::get_element(aname))
		return result;

	// fields

	XalanNode *self=&get_node(pool(), &aname);

	if(aname=="name") {
		return NEW VString(transcode(self->getNodeName()));
	} else if(aname=="value") {
		return NEW VString(transcode(self->getNodeValue()));
	} else if(aname=="type") {
		static const char *type_names[]={
			"unknown", "element", "attribute", "text", "cdata", "entityref",    
			"entity", "pi", "comment", "document", "doctype", "docfragment", 
			"notation"
		};
		XalanNode::NodeType node_type=self->getNodeType();
		if(node_type > sizeof(type_names)/sizeof(type_names[0]))
			node_type=XalanNode::UNKNOWN_NODE;
		return NEW VString(*NEW String(pool(), type_names[node_type]));
	} else if(aname=="parent") {
		if(XalanNode *result_node=self->getParentNode())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="first-child") {
		if(XalanNode *result_node=self->getFirstChild())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="last-child") {
		if(XalanNode *result_node=self->getLastChild())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="previous-sibling") {
		if(XalanNode *result_node=self->getPreviousSibling())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="next-sibling") {
		if(XalanNode *result_node=self->getNextSibling())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="owner") {
		if(XalanDocument *document=self->getOwnerDocument())
			return NEW VDom(pool(), document);
	} else switch(self->getNodeType()) {
		case XalanNode::ELEMENT_NODE: 
			if(aname=="attributes") {
				if(const XalanNamedNodeMap *attributes=self->getAttributes()) {
					VHash *result=NEW VHash(pool());
					for(int i=0; i<attributes->getLength(); i++) {
						XalanNode *attr_node=attributes->item(i);
						result->hash().put(
							transcode(attr_node->getNodeName()), 
							NEW VDnode(pool(), attr_node));
					}
					return result;
				}
			}
			break;
		case XalanNode::ATTRIBUTE_NODE: 
			if(aname=="specified")
				return NEW VBool(pool(), static_cast<XalanAttr *>(self)->getSpecified());
			break;
/*
		case XalanNode::TEXT_NODE: 
		case XalanNode::CDATA_SECTION_NODE: 
		case XalanNode::COMMENT_NODE: 
			substringData(unsigned int offset, unsigned int count)
*/
		//case XalanNode::ENTITY_REFERENCE_NODE: 
		//case XalanNode::ENTITY_NODE: 
		case XalanNode::PROCESSING_INSTRUCTION_NODE: 
			if(aname=="target")
				return NEW VString(transcode(static_cast<XalanProcessingInstruction *>(self)->getTarget()));
			break;
/*
		case XalanNode::DOCUMENT_NODE: 
virtual XalanDocumentType* getDoctype () const = 0 
	Get Document Type Declaration (see DocumentType) associated with this document. More...
virtual XalanDOMImplementation* getImplementation () const = 0 
	Return the DOMImplementation object that handles this document.
virtual XalanElement* getDocumentElement () const = 0 
	Return a reference to the root element of the document.
virtual XalanNodeList* getElementsByTagName (const XalanDOMString& tagname) const = 0 
	Returns a NodeList of all the elements with a given tag name. More...
*/
/*
		case XalanNode::DOCUMENT_TYPE_NODE: 
virtual const XalanDOMString& getName () const = 0 
	The name of DTD; i.e., the name immediately following the DOCTYPE keyword in an XML source document.
virtual const XalanNamedNodeMap* getEntities () const = 0 
	This function returns a NamedNodeMap containing the general entities, both external and internal, declared in the DTD. More...
virtual const XalanNamedNodeMap* getNotations () const = 0 
	This function returns a named node map containing an entry for each notation declared in a document's DTD. More...*/
//		case XalanNode::DOCUMENT_FRAGMENT_NODE: 
//		case XalanNode::NOTATION_NODE;
	}
	

	return 0;
}

#endif
