/** @file
	Parser: implementation of apr functions.

	Copyright(c) 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char* IDENT="$Date: 2003/11/06 08:49:56 $";

#include "apr_pools.h"


APR_DECLARE(void) apr_pool_cleanup_register(
    apr_pool_t *p,
    const void *data,
    apr_status_t (*plain_cleanup)(void *),
    apr_status_t (*child_cleanup)(void *))
{
}

APR_DECLARE_NONSTD(apr_status_t) apr_pool_cleanup_null(void *data)
{
	return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) apr_pool_cleanup_run(
    apr_pool_t *p,
    void *data,
    apr_status_t (*cleanup)(void *))
{
	return APR_SUCCESS;
}
