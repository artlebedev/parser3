/*
  $Id: pa_table.h,v 1.3 2001/01/29 20:46:22 paf Exp $
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

	// request I'm processed on
	Request& request;

	// the base origin of table data
	Origin origin;

	int current;

	// columns
	Array *columns;

	Table(Request& request,
		char *afile, uint aline,
		Array *acolumns,
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	const char *item(int column_index);
	const char *item(String column_name);

protected:

	// column name->number lookup table
	Hash name2number;

	const Array *at(int index);

};

#endif
