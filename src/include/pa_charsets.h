/** @file
	Parser: sql driver manager decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_charsets.h,v 1.1 2001/12/15 21:51:43 paf Exp $


	global sql driver manager, must be thread-safe
*/

#ifndef PA_CHARSETS_H
#define PA_CHARSETS_H

#include "pa_hash.h"

class Charsets : public Hash {
public:
	Charsets(Pool& apool);
	~Charsets();
};

/// global
extern Charsets *charsets;


#endif
