/** @file
	Parser: zero Mutex realization class.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_threads.C,v 1.4 2001/10/31 11:23:39 paf Exp $
*/

#include "pa_threads.h"

/// @test really?
const bool parser_multithreaded=false;

Mutex global_mutex;

Mutex::Mutex() {
}

Mutex::~Mutex() {
}

void Mutex::acquire() {
}

void Mutex::release() {
}
