/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: xnode.h,v 1.1 2001/09/26 11:24:07 parser Exp $
*/

#ifndef XNODE_H
#define XNODE_H

#include <util/XMLString.hpp>

class MXnode : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VXnode(pool); }

public:
	MXnode(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

#endif
