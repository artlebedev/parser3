/**	@file
	Parser: dictionary class  impl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT_DICTIONARY_C="$Date: 2003/07/24 11:31:23 $";

#include "pa_dictionary.h"
#include "pa_exception.h"

// could not make this static: gcc complains "prev decl was extern"
void pa_dictionary_add_first(Table::element_type row, Dictionary* self) {
	// get a=>b values
	const String* a=row->get(0);
	// empty 'a' check
	if(a->is_empty()) {
		throw Exception("parser.runtime",
			0, //&a, 
			"dictionary table 'from' column elements must not be empty");
	}
	unsigned char c=(unsigned char)a->first_char();
	if(!self->starting_line_of[c])
		self->starting_line_of[c]=self->constructor_line;

	self->constructor_line++;
}
Dictionary::Dictionary(Table& atable): table(atable) {
	// clear starting_lines
	memset(starting_line_of, 0, sizeof(starting_line_of));
	// grab first letters of first column of a table
	constructor_line=1;  table.for_each(pa_dictionary_add_first, this);
}

#ifndef DOXYGEN
struct First_that_begins_info {
	int line;
	const char* str;
};
#endif
static bool starts(Table::element_type row, First_that_begins_info* info) {
	// skip irrelevant lines
	if(info->line>1) {
		--info->line;
		return 0;
	}

	return row->get(0)->this_starts(info->str);
}
Table::element_type Dictionary::first_that_begins(const char* str) const {
	First_that_begins_info info;
	if(info.line=starting_line_of[(unsigned char)*str]) {
		info.str=str;
		return table.first_that(starts, &info);
	} else
		return Table::element_type(0);
}
