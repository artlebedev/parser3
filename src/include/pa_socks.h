/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

static const char* IDENT_SOCKS_H="$Date: 2002/08/01 11:41:16 $";

#include "pa_pool.h"

void init_socks(Pool& pool);
void done_socks();

#endif
