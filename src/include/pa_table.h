/*
  $Id: pa_table.h,v 1.1 2001/01/29 15:56:03 paf Exp $
*/

#ifndef PA_TABLE_H
#define PA_TABLE_H

#include "pa_types.h"
#include "pa_array.h"
#include "pa_hash.h"

class Table : public Array {
public:

	// the base origin of table data
	Origin origin;

	// columns order
	Array *columns_order;

	// column name->number lookup table
	// hence most of tables are "named", no need to uptimize unnamed onces
	Hash name2number;

	Table(Pool *apool, 
		char *afile, uint aline, 
		Array *acolumns, 
		int initial_rows=CR_INITIAL_ROWS_DEFAULT);
};

#endif
