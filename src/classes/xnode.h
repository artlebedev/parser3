/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef XNODE_H
#define XNODE_H

static const char* IDENT_XNODE_H="$Id: xnode.h,v 1.12 2002/08/01 11:26:44 paf Exp $";

class MXnode : public Methoded {
public: // Value

	/// MXnode: +$const
	Value *get_element(const String& aname) {
		// $CLASS, $method
		if(Value *result=Methoded::get_element(aname))
			return result;

		// $const
		if(Value *result=(Value *)consts.get(aname))
			return result;
		
		return 0;
	}


public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VXnode(pool, 0); }

public:
	MXnode(Pool& pool, const char *aname=0, VStateless_class *abase=0);

public: // Methoded
	bool used_directly() { return true; }

private:

	Hash consts;
};

#define gdome_xml_doc_get_xmlDoc(dome_doc) ((_Gdome_xml_Document *)dome_doc)->n;

GdomeNode *as_node(const String& method_name, MethodParams *params, 
						int index, const char *msg);

#endif
