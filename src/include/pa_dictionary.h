/** @file
	Parser: dictionary class decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DICTIONARY_H
#define PA_DICTIONARY_H

static const char* IDENT_DICTIONARY_H="$Date: 2003/10/03 09:40:57 $";

#include "pa_table.h"

/// simple dictionary, speding up lookups on contained two columned table
class Dictionary: public PA_Object {
public:

	struct Subst {
		const char *from; size_t from_length;
		const String* to;

		Subst(int): from(0) {}
		Subst(const char* afrom, const String* ato): from(afrom), to(ato) {
			from_length=strlen(afrom);
		}
		operator bool() { return from!=0; }
	};

	/// construct wrapper, grabbing first letters of first column into @b first
	Dictionary(Table& atable);

	/// find first row that contains string in first column which starts @b src
	Subst first_that_begins(const char* str) const;

private:

	Array<Subst> substs;

private:
	friend void pa_dictionary_add_first(Table::element_type row, Dictionary* self);
	//friend bool starts(FromTo row, First_that_begins_info* info);

	int starting_line_of[0x100]; int constructor_line;
};

#endif
