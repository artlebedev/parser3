/** @file
	Parser: pool class.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_pool.h"
#include "pa_exception.h"
#include "pa_common.h"
#include "pa_sapi.h"
#include "pa_charset.h"

volatile const char * IDENT_PA_POOL_C="$Id: pa_pool.C,v 1.66 2020/12/15 17:10:36 moko Exp $" IDENT_PA_POOL_H;

// Pool

Pool::Pool(){}

static void cleanup(Pool::Cleanup item, int) {
	if(item.cleanup)
		item.cleanup(item.data);
}
Pool::~Pool() {
	//__asm__("int3");
	//_asm int 3;
	//fprintf(stderr, "cleanups: %d\n", cleanups.size());
	// cleanups first, because they use some object's memory pointers
	cleanups.for_each(cleanup, 0);
}

void Pool::register_cleanup(void (*cleanup) (void *), void *data) {
	cleanups+=Cleanup(cleanup, data);
}

static void unregister_cleanup(Pool::Cleanup& item, void* cleanup_data) {
	if(item.data==cleanup_data)
		item.cleanup=0;
}
void Pool::unregister_cleanup(void *cleanup_data) {
	cleanups.for_each_ref(::unregister_cleanup, cleanup_data);
}

// Pooled

static void cleanup(void *data) {
	static_cast<Pooled*>(data)->~Pooled();
}

Pooled::Pooled(Pool& apool): fpool(apool) {
	fpool.register_cleanup(cleanup, this);
}

/// Sole: this got called automatically from Pool::~Pool()
Pooled::~Pooled() {
	fpool.unregister_cleanup(this);
}
