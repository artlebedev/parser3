/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef XNODE_H
#define XNODE_H

static const char * const IDENT_XNODE_H="$Date: 2009/06/14 00:33:36 $";

class MXnode: public Methoded {
public: // Value

	/// MXnode: +$const
	Value* get_element(const String& aname, Value& aself, bool looking_up) {
		// $method
		if(Value* result=Methoded::get_element(aname, aself, looking_up))
			return result;

		// $const
		if(Value* result=consts.get(aname))
			return result;
		
		return 0;
	}


public: // VStateless_class
	Value* create_new_value(Pool&, HashStringValue*) { 
		throw Exception(PARSER_RUNTIME,
			0,
			"no constructors available, use CreateXXX DOM methods to create nodes instead"); 
	}

public:
	MXnode(const char* aname=0, VStateless_class* abase=0);

public: // Methoded
	bool used_directly() { return true; }

private:

	HashStringValue consts;
};

xmlNode& as_node(MethodParams& params, int index, const char* msg);
xmlChar* as_xmlchar(Request& r, MethodParams& params, int index, const char* msg);
xmlNs& pa_xmlMapNs(xmlDoc& doc, const xmlChar* href, const xmlChar* prefix);

#endif
