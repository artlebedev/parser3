/*
  $Id: pa_pool.C,v 1.4 2001/01/30 11:51:07 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"

Pool::Pool() :
	fglobal_exception(0),	
	flocal_exception(0) {
}

Pool::~Pool() {
}

Exception *Pool::set_global_exception(Exception *e) {
	Exception *r=fglobal_exception;
	fglobal_exception=e;
	return r;
}
void Pool::restore_global_exception(Exception *e) {
	fglobal_exception=e;
}

Exception *Pool::set_local_exception(Exception *e) {
	Exception *r=flocal_exception;
	flocal_exception=e;
	return r;
}
void Pool::restore_local_exception(Exception *e) {
	flocal_exception=e;
}
