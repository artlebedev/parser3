/**	@file
	Parser: dictionary class  impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_dictionary.C,v 1.3 2001/08/06 16:18:26 parser Exp $"; 

#include "pa_dictionary.h"


static void add_first(Array::Item *value, void *info) {
	Dictionary& self=*static_cast<Dictionary *>(info);
	self.first[(unsigned char)static_cast<Array *>(value)->get_string(0)->first_char()]=true;
}

Dictionary::Dictionary(Table& atable) : Pooled(atable.pool()), table(atable) {
	// clear firsts
	memset(first, 0, sizeof(first));
	// grab first letters of first column of a table
	table.for_each(add_first, this);
}

static void *starts(Array::Item *value, const void *info) {
	Array *row=static_cast<Array *>(value);
	const char *src=static_cast<const char *>(info);

	int partial;
	row->get_string(0)->cmp(partial, src);
	return (
		partial==0 || // full match
		partial==1) // typo left column starts 'src'
		?value:0;
}

void* Dictionary::first_that_starts(const char *src) const {
	if(first[(unsigned char)*src])
		return table.first_that(starts, src);
	else
		return 0;
}
