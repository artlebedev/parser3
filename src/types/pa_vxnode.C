/** @node
	Parser: @b dnode parser type.

	Copyright(c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

static const char* IDENT_VXNODE_C="$Date: 2003/08/19 12:07:36 $";

#include "pa_vxnode.h"
#include "pa_vxdoc.h"
#include "pa_vstring.h"
#include "pa_vbool.h"
#include "pa_vhash.h"
#include "pa_request_charsets.h"
#include "pa_charset.h"

/// @todo strdup properties [now can be: $nodeName[$xdoc.nodeName], free(xdoc), >>print $nodeName<<GPF
Value* VXnode::get_element(const String& aname, Value& aself, bool looking_up) { 
	// $CLASS,$method
	if(Value* result=VStateless_object::get_element(aname, aself, looking_up))
		return result;

	// fields

	GdomeNode* selfNode=get_node();
	GdomeException exc;

	if(aname=="nodeName") {
		return new VString(fcharsets->source().transcode(gdome_n_nodeName(selfNode, &exc)));
	} else if(aname=="nodeValue") {
		return new VString(fcharsets->source().transcode(gdome_n_nodeValue(selfNode, &exc)));
	} else if(aname=="nodeType") {
		return new VInt(gdome_n_nodeType(selfNode, &exc));
	} else if(aname=="parentNode") {
		if(GdomeNode* result_node=gdome_n_parentNode(selfNode, &exc))
			return new VXnode(fcharsets, result_node);
		return 0;
	} else if(aname=="childNodes") {	
		if(GdomeNode* currentNode=gdome_n_firstChild(selfNode, &exc)) {
			VHash* result=new VHash;
			int i=0;
			do {
				result->hash().put(
					StringBody::Format(i++), 
					new VXnode(fcharsets, currentNode));
			} while(currentNode=gdome_n_nextSibling(currentNode, &exc));
			return result;
		}
		return 0;
	} else if(aname=="firstChild") {
		if(GdomeNode* result_node=gdome_n_firstChild(selfNode, &exc))
			return new VXnode(fcharsets, result_node);
		return 0;
	} else if(aname=="lastChild") {
		if(GdomeNode* result_node=gdome_n_lastChild(selfNode, &exc))
			return new VXnode(fcharsets, result_node);
		return 0;
	} else if(aname=="previousSibling") {
		if(GdomeNode* result_node=gdome_n_previousSibling(selfNode, &exc))
			return new VXnode(fcharsets, result_node);
		return 0;
	} else if(aname=="nextSibling") {
		if(GdomeNode* result_node=gdome_n_nextSibling(selfNode, &exc))
			return new VXnode(fcharsets, result_node);
		return 0;
	} else if(aname=="ownerDocument") {
		if(GdomeDocument *document=gdome_n_ownerDocument(selfNode, &exc))
			return new VXdoc(fcharsets, document);
		return 0;
	} else switch(gdome_n_nodeType(selfNode, &exc)) {
		case GDOME_ELEMENT_NODE: 
			if(aname=="attributes") {
				if(GdomeNamedNodeMap *attributes=gdome_n_attributes(selfNode, &exc)) {
					VHash* result=new VHash;
					gulong length=gdome_nnm_length(attributes, &exc);
					for(gulong i=0; i<length; i++) {
						GdomeNode* attr_node=gdome_nnm_item(attributes, i, &exc);
						result->hash().put(
							fcharsets->source().transcode(gdome_n_nodeName(attr_node, &exc)), 
							new VXnode(fcharsets, attr_node));
					}
					return result;
				}
				return 0;
			} else if(aname=="tagName") {
				return new VString(fcharsets->source().transcode(gdome_el_tagName(GDOME_EL(selfNode), &exc)));
			}
			break;
		case GDOME_ATTRIBUTE_NODE: 
			if(aname=="specified")
				return new VBool(gdome_a_specified(GDOME_A(selfNode), &exc)!=0);
			else if(aname=="name")
				return new VString(fcharsets->source().transcode(gdome_a_name(GDOME_A(selfNode), &exc)));
			else if(aname=="value")
				return new VString(fcharsets->source().transcode(gdome_a_value(GDOME_A(selfNode), &exc)));
			break;
/*
		case GDOME_COMMENT_NODE: 
			substringData(unsigned int offset, unsigned int count)
*/
		case GDOME_PROCESSING_INSTRUCTION_NODE: 
			if(aname=="target")
				return new VString(fcharsets->source().transcode(gdome_pi_target(GDOME_PI(selfNode), &exc)));
			else if(aname=="data")
				return new VString(fcharsets->source().transcode(gdome_pi_data(GDOME_PI(selfNode), &exc)));
			break;
		case GDOME_DOCUMENT_TYPE_NODE: 
			{
				GdomeDocumentType *doctype=GDOME_DT(selfNode);
				if(aname=="name") {
					// readonly attribute DOMString aname;
					// The aname of DTD; i.e., the aname immediately following 
					// the DOCTYPE keyword in an XML source document.
					return new VString(fcharsets->source().transcode(gdome_dt_name(doctype, &exc)));
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
				if(aname=="publicId") {
					// readonly attribute DOMString publicId;
					return new VString(fcharsets->source().transcode(gdome_not_publicId(notation, &exc)));
				} else if(aname=="systemId") {
					// readonly attribute DOMString systemId;
					return new VString(fcharsets->source().transcode(gdome_not_systemId(notation, &exc)));
				}
			}
			break;
	}
		
	return bark("%s field not found", &aname);
}

#endif
