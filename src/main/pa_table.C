/*
  $Id: pa_table.C,v 1.1 2001/01/29 15:56:04 paf Exp $
*/

#include "pa_table.h"
#include "pa_pool.h"

Table::Table(Pool *apool, 
			 char *afile, uint aline, 
			 Array *acolumns, 
			 int initial_rows) :
	Array(apool, initial_rows),
	columns_order(acolumns), 
	name2number(apool, false) {
#ifndef NO_STRING_ORIGIN
	origin.file=afile;
	origin.line=aline;
#endif

	if(columns_order)
		for(int i=0; i<columns_order->size(); i++) {
			String name(pool);
			name.APPEND(static_cast<char *>((*columns_order)[i]), 0, 0);
			name2number.put(name, reinterpret_cast<Item>(i));
		}
}
