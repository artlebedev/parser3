/**	@file
	Parser: socks initialization/finalization decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_socks.h,v 1.7 2002/02/08 07:27:45 paf Exp $
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#include "pa_pool.h"

void init_socks(Pool& pool);
void done_socks();

#endif
