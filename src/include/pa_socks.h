/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_socks.h,v 1.6 2001/12/15 21:28:20 paf Exp $
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#include "pa_pool.h"

void init_socks(Pool& pool);
void done_socks();

#endif
