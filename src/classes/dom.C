/** @file
	Parser: @b dom parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: dom.C,v 1.25 2001/09/18 16:05:42 parser Exp $"; 

#if _MSC_VER
#	pragma warning(disable:4291)   // disable warning 
//	"no matching operator delete found; memory will not be freed if initialization throws an exception
#endif

#include "classes.h"
#include "pa_request.h"
#include "pa_vdom.h"
#include "pa_xslt_stylesheet_manager.h"
#include "pa_stylesheet_connection.h"
#include "dnode.h"

#include <strstream>
#include <Include/PlatformDefinitions.hpp>
#include <util/PlatformUtils.hpp>
#include <XalanTransformer/XalanTransformer.hpp>
#include <XalanTransformer/XalanParsedSource.hpp>

// defines

#define DOM_CLASS_NAME "dom"

// class

class MDom : public MDnode {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VDom(pool); }

public:
	MDom(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

// methods

static void _set(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	Value& vxml=params->as_junction(0, "xml must be code");
	Temp_lang temp_lang(r, String::UL_XML);
	const String& xml=r.process(vxml).as_string();

	std::istrstream stream(xml.cstr());
	XalanParsedSource* parsedSource;
	int error=vDom.transformer().parseSource(&stream, parsedSource);

	if(error)
		PTHROW(0, 0,
			&method_name,
			vDom.transformer().getLastError());

	// replace any previous parsed source
	vDom.set_parsed_source(*parsedSource);
}

static void _load(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// filespec
	const String& file_name=params->as_string(0, "file name must not be code");
	const char *filespec=r.absolute(file_name).cstr(String::UL_FILE_SPEC);
	
	XalanParsedSource* parsedSource;
	int error=vDom.transformer().parseSource(filespec, parsedSource);

	if(error)
		PTHROW(0, 0,
			&file_name,
			vDom.transformer().getLastError());

	// replace any previous parsed source
	vDom.set_parsed_source(*parsedSource);
}

static void add_xslt_param(const Hash::Key& aattribute, Hash::Val *ameaning, 
						   void *info) {
	XalanTransformer& transformer=*static_cast<XalanTransformer *>(info);
	const char *attribute_cstr=aattribute.cstr();
	const char *meaning_cstr=static_cast<Value *>(ameaning)->as_string().cstr();

	transformer.setStylesheetParam(
		XalanDOMString(attribute_cstr),  
		XalanDOMString(meaning_cstr));
}
static void _xslt(Request& r, const String& method_name, MethodParams *params) {
	Pool& pool=r.pool();
	VDom& vDom=*static_cast<VDom *>(r.self);

	// params
	if(params->size()>1) {
		Value& vparams=params->as_no_junction(1, "transform parameters parameter must not be code");
		if(vparams.is_defined())
			if(Hash *params=vparams.get_hash())
				params->for_each(add_xslt_param, &vDom.transformer());
			else
				PTHROW(0, 0,
					&method_name,
					"transform parameters parameter must be hash");
	}

	// source
	XalanParsedSource &parsed_source=vDom.get_parsed_source(pool, &method_name);

	// stylesheet
	const String& stylesheet_file_name=params->as_string(0, "file name must not be code");
	const String& stylesheet_filespec=r.absolute(stylesheet_file_name);
	//_asm int 3;
	Stylesheet_connection& connection=XSLT_stylesheet_manager->get_connection(stylesheet_filespec);

	// target
	XalanDocument* target=vDom.parser_liaison().createDocument();
	XSLTResultTarget domResultTarget(target);

	// transform
	int error=vDom.transformer().transform(
		parsed_source, 
		&connection.stylesheet(), 
		domResultTarget);
	connection.close();
	if(error)
		PTHROW(0, 0,
			&stylesheet_file_name,
			vDom.transformer().getLastError());

	// write out result
	VDom& result=*new(pool) VDom(pool);
	result.set_document(*target);
	r.write_no_lang(result);
}

// constructor

MDom::MDom(Pool& apool) : MDnode(apool) {
	set_name(*NEW String(pool(), DOM_CLASS_NAME));

	// ^dom::set[<some>xml</some>]
	add_native_method("set", Method::CT_DYNAMIC, _set, 1, 1);

	// ^dom::load[some.xml]
	add_native_method("load", Method::CT_DYNAMIC, _load, 1, 1);

	// ^dom.xslt[stylesheet file_name]
	// ^dom.xslt[stylesheet file_name;params hash]
	add_native_method("xslt", Method::CT_DYNAMIC, _xslt, 1, 2);

}
// global variable

Methoded *Dom_class;

// creator

Methoded *MDom_create(Pool& pool) {
	return Dom_class=new(pool) MDom(pool);
}
