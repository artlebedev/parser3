/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_socks.h,v 1.4 2001/09/26 10:32:25 parser Exp $
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#include "pa_config_includes.h"
#include "pa_pool.h"

void init_socks(Pool& pool);
void done_socks();

#endif
