/** @node
	Parser: @b dnode parser type.

	Copyright(c) 2001 ArtLebedev Group(http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru>(http://design.ru/paf)

	$Id: pa_vdnode.C,v 1.2 2001/09/18 13:31:56 parser Exp $
*/

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "pa_vdnode.h"
#include "pa_vdom.h"
#include "pa_vstring.h"

#include <util/XMLString.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <XalanDOM/XalanNamedNodeMap.hpp>
/*
void VDnode_cleanup(void *vnode) {
	//_asm int 3;
	static_cast<VDDom *>(vnode)->cleanup();
}
*/

/// @test pool maxBytes
static const char *strX(const XalanDOMString& s) {
//	return XMLString::transcode(s.c_str());
	XMLTransService::Codes resValue;
	XMLTranscoder& transcoder=*XMLPlatformUtils::fgTransService->makeNewTranscoderFor(
		"windows-1251", resValue, 0x400);
	const unsigned int maxBytes=0x100;
	XMLByte* toFill=(XMLByte *)malloc(maxBytes*sizeof(XMLByte));
	unsigned int charsEaten;
	unsigned int size=transcoder.transcodeTo(
        s.c_str(), s.length(),
        toFill,
        maxBytes,
        charsEaten,
		XMLTranscoder::UnRep_RepChar /*UnRep_Throw*/
    );
	toFill[size]=0;
	return (const char *)toFill;
}

void _throw(Pool& pool, const String *source, const XSLException& e) {
	if(e.getURI().empty())
		PTHROW(0, 0,
			source,
			"%s (%s)",
				strX(e.getMessage()),  // message for exception
				strX(e.getType()) // type of exception
		);
	else
		PTHROW(0, 0,
			source,
			"%s (%s) %s(%d:%d)'", 
				strX(e.getMessage()),  // message for exception
				strX(e.getType()), // type of exception
				
				strX(e.getURI()),  // URI for the associated document, if any
				e.getLineNumber(),  // line number, or -1 if unknown
				e.getColumnNumber() // column number, or -1 if unknown
		);
}

/// VDnode: $CLASS,$method
Value *VDnode::get_element(const String& aname) { 
	// $CLASS,$method
	if(Value *result=VStateless_object::get_element(aname))
		return result;

	// fields

	XalanNode& self=get_node(pool(), &aname);

	if(aname=="name") {
		const char *name_cstr=strX(self.getNodeName());
		String& name=*NEW String(pool(), name_cstr);
		return NEW VString(name);
	} else if(aname=="value") {
		const char *value_cstr=strX(self.getNodeValue());
		String& value=*NEW String(pool(), value_cstr);
		return NEW VString(value);
	} else if(aname=="type") {
		static const char *type_names[]={
			"unknown",
			"element",    
			"attribute",    
			"text",    
			"cdata",    
			"entityref",    
			"entity",    
			"pi",    
			"comment",    
			"document",    
			"doctype",    
			"docfragment",    
			"notation"
		};
		XalanNode::NodeType node_type=self.getNodeType();
		if(node_type > sizeof(type_names)/sizeof(type_names[0]))
			node_type=XalanNode::UNKNOWN_NODE;
		return NEW VString(*NEW String(pool(), type_names[node_type]));
	} else if(aname=="parent") {
		if(XalanNode *result_node=self.getParentNode())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="first-child") {
		if(XalanNode *result_node=self.getFirstChild())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="last-child") {
		if(XalanNode *result_node=self.getLastChild())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="previous-sibling") {
		if(XalanNode *result_node=self.getPreviousSibling())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="next-sibling") {
		if(XalanNode *result_node=self.getNextSibling())
			return NEW VDnode(pool(), result_node);
	} else if(aname=="attributes") {
		if(const XalanNamedNodeMap *attributes=self.getAttributes()) {
			VHash *result=NEW VHash(pool());
			for(int i=0; i<attributes->getLength(); i++) {
				XalanNode *attr_node=attributes->item(i);
				result->hash().put(
					*NEW String(pool(), strX(attr_node->getNodeName())), 
					NEW VDnode(pool(), attr_node));
			}
			return result;
		}
	} else if(aname=="owner") {
		if(XalanDocument *document=self.getOwnerDocument())
			return NEW VDom(pool(), document);
	} 
	

	return 0;
}
