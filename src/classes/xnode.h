/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: xnode.h,v 1.7 2002/01/28 10:33:52 paf Exp $
*/

#ifndef XNODE_H
#define XNODE_H


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
	MXnode(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }

private:

	Hash consts;
};

#define gdome_xml_doc_get_xmlDoc(dome_doc) ((_Gdome_xml_Document *)dome_doc)->n;

GdomeNode *as_node(Pool& pool, const String& method_name, MethodParams *params, 
						int index, const char *msg);

#endif
