/**	@file
	Parser: dictionary class  impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_dictionary.C,v 1.9 2001/11/05 11:46:27 paf Exp $
*/

#include "pa_dictionary.h"
#include "pa_exception.h"

void Dictionary::add_first(Array::Item *value, void *info) {
	Dictionary& self=*static_cast<Dictionary *>(info);
	Array *row=static_cast<Array *>(value);
	// get a=>b values
	const String& a=*row->get_string(0);
	const String& b=*row->get_string(1);
	// empty 'a' check
	if(a.size()==0) {
		Pool& pool=self.pool();
		throw Exception(0, 0, 
			&a, 
			"dictionary table 'from' column elements must not be empty");
	}
	unsigned char c=(unsigned char)a.first_char();
	if(!self.starting_line_of[c])
		self.starting_line_of[c]=self.constructor_line;

	double ratio=((double)b.size())/a.size();
	if(ratio>self.fmax_ratio)
		self.fmax_ratio=ratio;

	self.constructor_line++;
}

Dictionary::Dictionary(Table& atable, double amin_ratio) : Pooled(atable.pool()), 
	table(atable), 
	fmax_ratio(amin_ratio)  {

	// clear starting_lines
	memset(starting_line_of, 0, sizeof(starting_line_of));
	// grab first letters of first column of a table
	constructor_line=1;  table.for_each(add_first, this);
}

#ifndef DOXYGEN
struct First_that_starts_info {
	int line;
	const char *src;  size_t src_size;
};
#endif
static void *starts(Array::Item *value, void *info) {
	First_that_starts_info& i=*static_cast<First_that_starts_info *>(info);
	// skip irrelevant lines
	if(i.line>1) {
		--i.line;
		return 0;
	}
	Array *row=static_cast<Array *>(value);

	int partial;
	row->get_string(0)->cmp(partial, i.src, i.src_size);
	return (
		partial==0 || // full match
		partial==1) // typo left column starts 'src'
		?value:0;
}

void* Dictionary::first_that_starts(const char *src, size_t src_size) const {
	First_that_starts_info info;
	if(info.line=starting_line_of[(unsigned char)*src]) {
		info.src=src;
		info.src_size=src_size;
		return table.first_that(starts, &info);
	} else
		return 0;
}
