/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: xnode.h,v 1.2 2001/09/26 15:43:59 parser Exp $
*/

#ifndef XNODE_H
#define XNODE_H

#include <util/XMLString.hpp>

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
	Value *create_new_value(Pool& pool) { return new(pool) VXnode(pool); }

public:
	MXnode(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }

private:

	Hash consts;
};

#endif
