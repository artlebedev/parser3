/**	@file
	Parser: dictionary class  impl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT_DICTIONARY_C="$Date: 2003/11/20 17:46:02 $";

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
	const String* b;
	if(row->count()>1) { // are there any b?
		b=row->get(1);
		if(b->is_empty())
			b=0;
	} else
		b=0;

	// record simplier 'a' for quick comparisons in 'starts' extremely-tight-callback
	self->substs+=Dictionary::Subst(a->cstr(), b);

	unsigned char c=(unsigned char)a->first_char();
	if(!self->starting_line_of[c])
		self->starting_line_of[c]=self->constructor_line;

	self->constructor_line++;
}
Dictionary::Dictionary(Table& atable): substs(atable.count()) {
	// clear starting_lines
	memset(starting_line_of, 0, sizeof(starting_line_of));
	// grab first letters of first column of a table
	constructor_line=1;  atable.for_each(pa_dictionary_add_first, this);
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
