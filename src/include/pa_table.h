/*
  $Id: pa_table.h,v 1.5 2001/01/29 22:34:57 paf Exp $
*/

/*
	hence most of tables are "named", no need to uptimize unnamed onces
*/


#ifndef PA_TABLE_H
#define PA_TABLE_H

#include "pa_types.h"
#include "pa_array.h"
#include "pa_hash.h"
#include "pa_string.h"
#include "pa_request.h"

class Table : public Array {
public:

	Table(Request& request,
		char *afile, uint aline,
		Array *acolumns,
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	// the base origin of table's data
	//const Origin& origin() { return forigin; }

	const Array *columns() { return fcolumns; }

	void set_current(int acurrent) { fcurrent=acurrent; }
	int get_current() { return fcurrent; }
	void inc_current() { fcurrent++; }

	void read_item(String& result, const String& column_name) {
		result.APPEND(item(column_name), forigin.file, forigin.line+fcurrent);
	}

protected:

	// request I'm processed on
	Request& request;

	// the base origin of table's data
	Origin forigin;

	// column name->number lookup table
	Hash name2number;

	// current row
	int fcurrent;

	// columns
	Array *fcolumns;

	const Array *at(int index);

	const char *item(int column_index);
	const char *item(const String& column_name);
};

#endif
