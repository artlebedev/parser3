/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_pool.C,v 1.2 2001/03/21 14:06:49 paf Exp $
*/

#include <stdlib.h>

#include "httpd.h"

#include "pa_pool.h"

void *Pool::real_malloc(size_t size) {
	return ap_palloc((pool*)fstorage, size);
}

void *Pool::real_calloc(size_t size) {
	return ap_pcalloc((pool*)fstorage, size/*, 1*/);
}
