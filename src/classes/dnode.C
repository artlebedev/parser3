/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
#include "classes.h"
#ifdef XML

static const char *RCSId="$Id: dnode.C,v 1.5 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_request.h"
#include "pa_vdnode.h"

#include "dnode.h"

#include <util/XMLString.hpp>
#include <XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <XPath/XPathEvaluator.hpp>
#include <XPath/NodeRefList.hpp>

// defines

#define DNODE_CLASS_NAME "dnode"

// class

// methods

static void _select(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VDnode& vnode=*static_cast<VDnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must not be code");
	const char *expression_cstr=expression.cstr(String::UL_AS_IS);
	XalanDOMString dstring(expression_cstr);
	const XalanDOMChar *expression_dcstr=dstring.c_str();

	XPathEvaluator evaluator;
	// Initialize the XalanSourceTree subsystem...
//	XalanSourceTreeInit		theSourceTreeInit;
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

			result.hash().put(skey, new(pool) VDnode(pool, list.item(i)));
		}
		result.set_name(method_name);
		r.write_no_lang(result);
	} catch(const XSLException& e) {
		r._throw(&expression, e);
	}
}

static void _select_single(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VDnode& vnode=*static_cast<VDnode *>(r.self);

	// expression
	const String& expression=params->as_string(0, "expression must not be code");
	const char *expression_cstr=expression.cstr(String::UL_AS_IS);
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

			VDnode& result=*new(pool) VDnode(pool, node);
			result.set_name(method_name);
			r.write_no_lang(result);
		}
	} catch(const XSLException& e) {
		r._throw(&expression, e);
	}
}

// constructor

MDnode::MDnode(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DNODE_CLASS_NAME));

	// ^node.select[/some/xpath/query] = hash $.#[dnode]
	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 1);

	// ^node.select-single[/some/xpath/query] = first dnode
	add_native_method("select-single", Method::CT_DYNAMIC, _select_single, 1, 1);

}
// global variable

Methoded *Dnode_class;

#endif

// creator
Methoded *MDnode_create(Pool& pool) {
	return 
#ifdef XML
		Dnode_class=new(pool) MDnode(pool)
#else
		0
#endif
		;
}
