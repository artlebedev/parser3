/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)
*/
static const char *RCSId="$Id: pa_threads.C,v 1.2 2001/09/21 14:46:09 parser Exp $"; 

#include "pa_threads.h"

Mutex global_mutex;

Mutex::Mutex() {
}

Mutex::~Mutex() {
}

void Mutex::acquire() {
}

void Mutex::release() {
}
