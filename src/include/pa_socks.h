/** @file
	Parser: executor part of request class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_socks.h,v 1.1 2001/04/07 14:23:31 paf Exp $
*/

#ifndef PA_SOCKS_H
#define PA_SOCKS_H

#include "pa_pool.h"

void init_socks(Pool& pool);
void done_socks();

#endif
