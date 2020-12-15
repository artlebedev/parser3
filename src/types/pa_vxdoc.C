/** @dom
	Parser: @b dom parser type.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/
#include "pa_config_includes.h"
#ifdef XML

#include "pa_vxdoc.h"
#include "pa_vbool.h"
#include "pa_request.h"
#include "pa_charset.h"

volatile const char * IDENT_PA_VXDOC_C="$Id: pa_vxdoc.C,v 1.52 2020/12/15 17:10:44 moko Exp $" IDENT_PA_VXDOC_H;

// defines

#define SEARCH_NAMESPACES_NAME "search-namespaces"

#define XDOC_OUTPUT_METHOD_OPTION_NAME "method"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_XML "xml"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML "html"
#define XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT "text"
#define XDOC_OUTPUT_FILENAME_OPTION_NAME "name"

VXnode& VXdoc::wrap(xmlNode& anode) {
	VXnode* result;
	if((result=static_cast<VXnode*>(anode._private))) {
		assert(anode.doc==fdocument);
		return *result;
	}

	result=new VXnode(anode);
	anode._private=result;
	anode.doc=fdocument;

	return *result;
}


Value* VXdoc::as(const char* atype) {
	return atype && ( strcmp(VXdoc::type(), atype)==0 || strcmp(VXnode::type(), atype)==0 )?this:0;
}

/// VXdoc: true	 
Value& VXdoc::as_expr_result() { return VBool::get(as_bool()); }


/// VXdoc: $method
Value* VXdoc::get_element(const String& aname) { 
	if(aname==SEARCH_NAMESPACES_NAME) {
		return &search_namespaces;
	}

	// up
	try {
		return VXnode::get_element(aname);
	} catch(Exception) { 
		// ignore bad node elements, they can be valid here...

		// fields
		xmlDoc& xmldoc=get_xmldoc();

		if(aname=="doctype") {
			// readonly attribute DocumentType doctype;
			if(xmlNode* node=(xmlNode*)xmldoc.intSubset)
				return &wrap(*node);
			else
				return 0;
		} else if(aname=="implementation") {
			// readonly attribute DOMImplementation implementation;
			return 0;
		} else if(aname=="documentElement") {
			// readonly attribute Element documentElement;
			xmlNode* rootElement=xmlDocGetRootElement(&xmldoc);
			return rootElement ? &wrap(*rootElement) : 0;
		}

		return bark("%s field not found", &aname);
	}
}

static int param_option_over_output_option(
						HashStringValue& param_options, const char* option_name,
						const String*& output_option) {
	if(Value* value=param_options.get(option_name)){
		output_option=&value->as_string();
		return 1;
	}
	return 0;
}
static int param_option_over_output_option(
						HashStringValue& param_options, const char* option_name,
						int& output_option) {
	if(Value* value=param_options.get(String::Body(option_name))) {
		const String& s=value->as_string();
		if(s=="yes")
			output_option=1;
		else if(s=="no")
			output_option=0;
		else
			throw Exception(PARSER_RUNTIME,
				&s,
				"%s must be either 'yes' or 'no'", option_name);
		return 1;
	}
	return 0;
}

void XDocOutputOptions::append(Request& r, HashStringValue* options, bool with_filename){
/*
<xsl:output
  !method = "xml" | "html" | "text" | qname-but-not-ncname 
  !version = nmtoken 
  !encoding = string 
  !omit-xml-declaration = "yes" | "no"
  !standalone = "yes" | "no"
  !doctype-public = string 
  !doctype-system = string 
  cdata-section-elements = qnames 
  !indent = "yes" | "no"
  !media-type = string /> 
*/

	if(options) {
		int valid_options=0;
		// $.charset[windows-1251|...]
		valid_options+=param_option_over_output_option(*options, "charset", this->encoding);
		// $.encoding[windows-1251|...]
		valid_options+=param_option_over_output_option(*options, "encoding", this->encoding);
		if(valid_options==2)
			throw Exception(PARSER_RUNTIME, 0, "you can not specify $.charset and $.encoding together");
		// $.method[xml|html|text]
		valid_options+=param_option_over_output_option(*options, XDOC_OUTPUT_METHOD_OPTION_NAME, this->method);
		// $.version[1.0]
		valid_options+=param_option_over_output_option(*options, "version", this->version);
		// $.omit-xml-declaration[yes|no]
		valid_options+=param_option_over_output_option(*options, "omit-xml-declaration", this->omitXmlDeclaration);
		// $.standalone[yes|no]
		valid_options+=param_option_over_output_option(*options, "standalone", this->standalone);
		// $.indent[yes|no]
		valid_options+=param_option_over_output_option(*options, "indent", this->indent);
		// $.media-type[text/{html|xml|plain}]
		valid_options+=param_option_over_output_option(*options, "media-type", this->mediaType);
		if(with_filename)
			// $.name[file name]
			valid_options+=param_option_over_output_option(*options, XDOC_OUTPUT_FILENAME_OPTION_NAME, this->filename);

		if(valid_options!=options->count())
			throw Exception(PARSER_RUNTIME, 0, CALLED_WITH_INVALID_OPTION);
	}

	// default encoding from pool
	if(!this->encoding)
		this->encoding=new String(r.charsets.source().NAME(), String::L_TAINTED);
	// default method=xml
	if(!this->method)
		this->method=new String(XDOC_OUTPUT_METHOD_OPTION_VALUE_XML);
	// default mediaType = depending on method
	if(!this->mediaType) {
		if(*this->method==XDOC_OUTPUT_METHOD_OPTION_VALUE_XML)
			this->mediaType=new String("text/xml");
		else if(*this->method==XDOC_OUTPUT_METHOD_OPTION_VALUE_HTML)
			this->mediaType=new String("text/html");
		else // XDOC_OUTPUT_METHOD_OPTION_VALUE_TEXT & all others
			this->mediaType=new String("text/plain");
	}
}

// defined at classes/xdoc.C
String::C xdoc2buf(Request& r, VXdoc& vdoc, 
					XDocOutputOptions& oo,
					const String* file_spec,
					bool use_source_charset_to_render_and_client_charset_to_write_to_header);

const String* VXdoc::get_json_string(Json_options& options){
	XDocOutputOptions xdoc_options_default;
	String::C buf=xdoc2buf(*options.r, *this, options.xdoc_options ? *options.xdoc_options : xdoc_options_default,
		0/*file_name. not to file, to memory*/,
		true/*use source charset to render, client charset to put to header*/);

	String& result=*new String("\"", String::L_AS_IS);
	result << String(buf, String::L_JSON);
	result << "\"";
	return &result;
}

#endif
