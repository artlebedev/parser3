/** @node
	Parser: @b dnode parser type.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

static const char* IDENT_VXNODE_C="$Date: 2002/08/01 11:41:25 $";

#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vstring.h"
#include "pa_vbool.h"
#include "pa_vhash.h"

void VXnode_cleanup(void *vxnode) {
	//_asm int 3;
	static_cast<VXnode *>(vxnode)->~VXnode();
}

Value *VXnode::get_element(const String& name) { 
	// $CLASS,$method
	if(Value *result=VStateless_object::get_element(name))
		return result;

	// fields

	GdomeNode *selfNode=get_node(&name);
	GdomeException exc;

	if(name=="nodeName") {
		return NEW VString(transcode(gdome_n_nodeName(selfNode, &exc)));
	} else if(name=="nodeValue") {
		return NEW VString(transcode(gdome_n_nodeValue(selfNode, &exc)));
	} else if(name=="nodeType") {
		return NEW VInt(pool(), gdome_n_nodeType(selfNode, &exc));
	} else if(name=="parentNode") {
		if(GdomeNode *result_node=gdome_n_parentNode(selfNode, &exc))
			return NEW VXnode(pool(), result_node);
	} else if(name=="childNodes") {	
		if(GdomeNode *currentNode=gdome_n_firstChild(selfNode, &exc)) {
			VHash *result=NEW VHash(pool());
			int i=0;
			do {
				String& skey=*NEW String(pool());
				{
					char *buf=(char *)malloc(MAX_NUMBER);
					snprintf(buf, MAX_NUMBER, "%d", ++i);
					skey << buf;
				}
				result->hash(&name).put(skey, NEW VXnode(pool(), currentNode));
			} while(currentNode=gdome_n_nextSibling(currentNode, &exc));
			return result;
		}
	} else if(name=="firstChild") {
		if(GdomeNode *result_node=gdome_n_firstChild(selfNode, &exc))
			return NEW VXnode(pool(), result_node);
	} else if(name=="lastChild") {
		if(GdomeNode *result_node=gdome_n_lastChild(selfNode, &exc))
			return NEW VXnode(pool(), result_node);
	} else if(name=="previousSibling") {
		if(GdomeNode *result_node=gdome_n_previousSibling(selfNode, &exc))
			return NEW VXnode(pool(), result_node);
	} else if(name=="nextSibling") {
		if(GdomeNode *result_node=gdome_n_nextSibling(selfNode, &exc))
			return NEW VXnode(pool(), result_node);
	} else if(name=="ownerDocument") {
		if(GdomeDocument *document=gdome_n_ownerDocument(selfNode, &exc))
			return NEW VXdoc(pool(), document);
	} else switch(gdome_n_nodeType(selfNode, &exc)) {
		case GDOME_ELEMENT_NODE: 
			if(name=="attributes") {
				if(GdomeNamedNodeMap *attributes=gdome_n_attributes(selfNode, &exc)) {
					VHash *result=NEW VHash(pool());
					gulong length=gdome_nnm_length(attributes, &exc);
					for(gulong i=0; i<length; i++) {
						GdomeNode *attr_node=gdome_nnm_item(attributes, i, &exc);
						result->hash(0).put(
							transcode(gdome_n_nodeName(attr_node, &exc)), 
							NEW VXnode(pool(), attr_node));
					}
					return result;
				}
			} else if(name=="tagName") {
				return NEW VString(transcode(gdome_el_tagName(GDOME_EL(selfNode), &exc)));
			}
			break;
		case GDOME_ATTRIBUTE_NODE: 
			if(name=="specified")
				return NEW VBool(pool(), gdome_a_specified(GDOME_A(selfNode), &exc)!=0);
			else if(name=="name")
				return NEW VString(transcode(gdome_a_name(GDOME_A(selfNode), &exc)));
			else if(name=="value")
				return NEW VString(transcode(gdome_a_value(GDOME_A(selfNode), &exc)));			
			break;
/*
		case GDOME_COMMENT_NODE: 
			substringData(unsigned int offset, unsigned int count)
*/
		case GDOME_PROCESSING_INSTRUCTION_NODE: 
			if(name=="target")
				return NEW VString(transcode(gdome_pi_target(GDOME_PI(selfNode), &exc)));
			else if(name=="data")
				return NEW VString(transcode(gdome_pi_data(GDOME_PI(selfNode), &exc)));				
			break;
		case GDOME_DOCUMENT_TYPE_NODE: 
			{
				GdomeDocumentType *doctype=GDOME_DT(selfNode);
				if(name=="name") {
					// readonly attribute DOMString name;
					// The name of DTD; i.e., the name immediately following 
					// the DOCTYPE keyword in an XML source document.
					return NEW VString(transcode(gdome_dt_name(doctype, &exc)));
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
		case GDOME_NOTATION_NODE:
			{
				GdomeNotation *notation=GDOME_NOT(selfNode);
				if(name=="publicId") {
					// readonly attribute DOMString publicId;
					return NEW VString(transcode(gdome_not_publicId(notation, &exc)));
				} else if(name=="systemId") {
					// readonly attribute DOMString systemId;
					return NEW VString(transcode(gdome_not_systemId(notation, &exc)));
				}
			}
			break;
	}
		
	return 0;
}

#endif
