/** @file
	Parser: dictionary class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DICTIONARY_H
#define PA_DICTIONARY_H

static const char* IDENT_DICTIONARY_H="$Date: 2003/07/24 11:31:21 $";

#include "pa_table.h"

/// simple dictionary, speding up lookups on contained two columned table
class Dictionary: public PA_Object {
public:

	/// construct wrapper, grabbing first letters of first column into @b first
	Dictionary(Table& atable);

	/// find first row that contains string in first column which starts @b src
	Table::element_type first_that_begins(const char* str) const;

private:

	Table& table;

private:
	friend void pa_dictionary_add_first(Table::element_type row, Dictionary* self);

	int starting_line_of[0x100]; int constructor_line;
};

#endif
