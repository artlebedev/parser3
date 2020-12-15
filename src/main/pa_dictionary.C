/**	@file
	Parser: dictionary class  impl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_dictionary.h"
#include "pa_exception.h"

volatile const char * IDENT_PA_DICTIONARY_C="$Id: pa_dictionary.C,v 1.29 2020/12/15 17:10:35 moko Exp $" IDENT_PA_DICTIONARY_H;

Dictionary::Dictionary(Table& atable): substs(atable.count()) {
	// clear starting_lines
	memset(starting_line_of, 0, sizeof(starting_line_of));
	// grab first letters of first column of a table
	constructor_line=1;

	for(Array_iterator<ArrayString*> i(atable); i.has_next(); ) {
		ArrayString* row=i.next();

		append_subst(
			row->get(0),
			(row->count()>1) ? row->get(1) : 0,
			"dictionary table 'from' column elements must not be empty"
		);
	}
}

Dictionary::Dictionary(const String& from, const String& to): substs(1) {
	// clear starting_lines
	memset(starting_line_of, 0, sizeof(starting_line_of));
	constructor_line=1;

	append_subst(&from, &to);
}


void Dictionary::append_subst(const String* from, const String* to, const char* exception_cstr) {
	if(from->is_empty())
		throw Exception(PARSER_RUNTIME,
			0,
			exception_cstr ? exception_cstr : "'from' must not be empty");

	// record simplier 'from' for quick comparisons in 'starts' extremely-tight-callback
	substs+=Dictionary::Subst(from->cstr(), (to && !to->is_empty()) ? to : 0);

	unsigned char c=(unsigned char)from->first_char();
	if(!starting_line_of[c])
		starting_line_of[c]=constructor_line;

	constructor_line++;
}

#ifndef DOXYGEN
struct First_that_begins_info {
	int line;
	const char* str;
};
#endif
static bool starts(Dictionary::Subst subst, First_that_begins_info* info) {
	// skip irrelevant lines
	if(info->line>1) {
		--info->line;
		return 0;
	}

	return strncmp(subst.from, info->str, subst.from_length)==0;
}
Dictionary::Subst Dictionary::first_that_begins(const char* str) const {
	First_that_begins_info info;
	if((info.line=starting_line_of[(unsigned char)*str])) {
		info.str=str;
		return substs.first_that(starts, &info);
	} else
		return 0;
}
