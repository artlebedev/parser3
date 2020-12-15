/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef XNODE_H
#define XNODE_H

#define IDENT_XNODE_H "$Id: xnode.h,v 1.35 2020/12/15 17:10:30 moko Exp $"

class MXnode: public Methoded {
public: // Value

	/// MXnode: +$const
	Value* get_element(const String& aname) {
		// $method
		if(Value* result=Methoded::get_element(aname))
			return result;

		// $const
		if(Value* result=consts.get(aname))
			return result;
		
		return 0;
	}


public: // VStateless_class
	Value* create_new_value(Pool&) { 
		throw Exception(PARSER_RUNTIME,
			0,
			"no constructors available, use CreateXXX DOM methods to create nodes instead"); 
	}

public:
	MXnode(const char* aname=0);

public: // Methoded
	bool used_directly() { return true; }

private:

	HashStringValue consts;
};

xmlNode& as_node(MethodParams& params, int index, const char* msg);
xmlChar* as_xmlchar(Request& r, MethodParams& params, int index, const char* msg);
xmlChar* as_xmlqname(Request& r, MethodParams& params, int index, const char* msg=0);
xmlChar* as_xmlncname(Request& r, MethodParams& params, int index, const char* msg=0);
xmlChar* as_xmlname(Request& r, MethodParams& params, int index, const char* msg=0);
xmlChar* as_xmlnsuri(Request& r, MethodParams& params, int index);
xmlNs& pa_xmlMapNs(xmlDoc& doc, const xmlChar* href, const xmlChar* prefix);

#endif
