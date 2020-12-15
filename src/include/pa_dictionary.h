/** @file
	Parser: dictionary class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DICTIONARY_H
#define PA_DICTIONARY_H

#define IDENT_PA_DICTIONARY_H "$Id: pa_dictionary.h,v 1.23 2020/12/15 17:10:30 moko Exp $"

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

	/// construct simple dictionary within a single pair only
	Dictionary(const String& from, const String& to);

	/// find first row that contains string in first column which starts @b src
	Subst first_that_begins(const char* str) const;

private:
	Array<Subst> substs;

private:
	void append_subst(const String* from, const String* to, const char* exception=0);

	int starting_line_of[0x100]; int constructor_line;

public:
	size_t count() const { return substs.count(); }

	Subst get(size_t index) const { return substs.get(index); }

};

#endif
