/** @file
	Parser: @b dnode methods class - MDnode class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: dnode.h,v 1.1 2001/09/18 12:25:06 parser Exp $
*/

#ifndef DNODE_H
#define DNODE_H

#include <util/XMLString.hpp>

class MDnode : public Methoded {
public: // VStateless_class
	Value *create_new_value(Pool& pool) { return new(pool) VDnode(pool); }

public:
	MDnode(Pool& pool);

public: // Methoded
	bool used_directly() { return true; }
};

#endif
