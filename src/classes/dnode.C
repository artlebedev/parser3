/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dnode.C,v 1.1 2001/09/18 12:25:06 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdnode.h"
#include "dnode.h"

#include <util/XMLString.hpp>
//#include <XalanSourceTree/XalanSourceTreeInit.hpp>
#include <XalanSourceTree/XalanSourceTreeDOMSupport.hpp>
#include <XPath/XPathEvaluator.hpp>
#include <XPath/NodeRefList.hpp>

// defines

#define DNODE_CLASS_NAME "dnode"

// class

// methods

static void _xpath(Request& r, const String& method_name, MethodParams *params) {
//	_asm int 3;
	Pool& pool=r.pool();
	VDnode& vdnode=*static_cast<VDnode *>(r.self);

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
			&vdnode.get_node(pool, &method_name), 
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
		_throw(pool, &expression, e);
	}
}

// constructor

MDnode::MDnode(Pool& apool) : Methoded(apool) {
	set_name(*NEW String(pool(), DNODE_CLASS_NAME));

	// ^node.xpath[/some/xpath/query]
	add_native_method("xpath", Method::CT_DYNAMIC, _xpath, 1, 1);

}
// global variable

Methoded *Dnode_class;

// creator

Methoded *MDnode_create(Pool& pool) {
	return Dnode_class=new(pool) MDnode(pool);
}
