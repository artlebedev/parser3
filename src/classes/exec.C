/** @file
	Parser: @b exec parser class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: exec.C,v 1.3 2001/04/09 11:30:35 paf Exp $
*/

#include "pa_config_includes.h"
#include "pa_sapi.h"
#include "_exec.h"
#include "pa_request.h"

VStateless_class *exec_class;


// initialize

void initialize_exec_class(Pool& pool, VStateless_class& vclass) {
}
