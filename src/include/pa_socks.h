/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_socks.h,v 1.2 2001/04/26 15:14:10 paf Exp $
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#include "pa_pool.h"

void init_socks(Pool& pool);
void done_socks();

#endif
