/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: xnode.C,v 1.1 2001/09/26 11:24:07 parser Exp $
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

// defines

#define XNODE_CLASS_NAME "xnode"

// class

// methods

static void _select(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VXnode& vnode=*static_cast<VXnode *>(r.self);

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

			result.hash().put(skey, new(pool) VXnode(pool, list.item(i)));
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
	VXnode& vnode=*static_cast<VXnode *>(r.self);

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

			VXnode& result=*new(pool) VXnode(pool, node);
			result.set_name(method_name);
			r.write_no_lang(result);
		}
	} catch(const XSLException& e) {
		r._throw(&expression, e);
	}
}

// constructor

MXnode::MXnode(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), XNODE_CLASS_NAME));

	// ^node.select[/some/xpath/query] = hash $.#[dnode]
	add_native_method("select", Method::CT_DYNAMIC, _select, 1, 1);

	// ^node.select-single[/some/xpath/query] = first dnode
	add_native_method("select-single", Method::CT_DYNAMIC, _select_single, 1, 1);

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
