/** @file
	Parser: dictionary class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_dictionary.h,v 1.1 2001/08/01 12:08:39 parser Exp $
*/

#ifndef PA_DICTIONARY_H
#define PA_DICTIONARY_H

#include "pa_config_includes.h"
#include "pa_table.h"

/// simple dictionary, speding up lookups on contained two columned table
class Dictionary : public Pooled {
public:

	/// construct wrapper, grabbing first letters of first column into @b first
	Dictionary(Table& atable);

	/// find first row that contains string in first column which starts @b src
	void* first_that_starts(const char *src) const;

	/// where this table came from, may be NULL. proxy to Table
	const String *origin_string() { return table.origin_string(); }

public:

	bool first[0x100];

private:

	Table& table;

};

#endif
