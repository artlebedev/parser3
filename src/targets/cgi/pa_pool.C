/** @file
	Parser: CGI memory manager impl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_pool.C,v 1.32 2001/11/08 14:08:54 paf Exp $
*/

#include <stdlib.h>

#include "pa_pool.h"
#include "pool_storage.h"

//#define DEBUG_POOL_MALLOC

#ifdef DEBUG_POOL_MALLOC
#include "pa_sapi.h"
#include "pa_common.h"
#include "pa_value.h"

#include "pa_array.h"
#include "pa_common.h"
#include "pa_dir.h"
#include "pa_exception.h"
#include "pa_exec.h"
#include "pa_globals.h"
#include "pa_hash.h"
#include "pa_opcode.h"
#include "pa_pool.h"
#include "pa_request.h"
#include "pa_sapi.h"
#include "pa_socks.h"
#include "pa_sql_connection.h"
#include "pa_sql_driver_manager.h"
#include "pa_stack.h"
#include "pa_string.h"
#include "pa_table.h"
#include "pa_threads.h"
#include "pa_types.h"
#include "pa_version.h"

#include "pa_valiased.h"
#include "pa_value.h"
#include "pa_vbool.h"
#include "pa_vclass.h"
#include "pa_vcode_frame.h"
#include "pa_vcookie.h"
#include "pa_vdouble.h"
#include "pa_venv.h"
#include "pa_vfile.h"
#include "pa_vform.h"
#include "pa_vhash.h"
#include "pa_vimage.h"
#include "pa_vint.h"
#include "pa_vjunction.h"
#include "pa_vmethod_frame.h"
#include "pa_vobject.h"
#include "pa_vrequest.h"
#include "pa_vresponse.h"
#include "pa_vstateless_class.h"
#include "pa_vstateless_object.h"
#include "pa_vstring.h"
#include "pa_vtable.h"
#include "pa_vvoid.h"
#include "pa_wcontext.h"
#include "pa_wwrapper.h"
#include "pa_dictionary.h"

#define MALLOC_STAT_MAXSIZE (0x400*0x400)
#define MALLOC_STAT_PLACES 20

int malloc_times[MALLOC_STAT_PLACES][MALLOC_STAT_MAXSIZE];
int malloc_places[MALLOC_STAT_PLACES];

void log_pool_stats(Pool& pool) {
	#define ST(type_name) \
		SAPI::log(pool, \
			"%4d %s", \
			sizeof(type_name), #type_name);
	ST(Value);
ST( Array);
ST(Exception);
ST(ffblk);
ST(Hash);
ST(Junction);
ST(Method);
ST(Methoded);
ST(Methoded_array);
ST(MethodParams);
ST(Operation);
ST(Origin);
ST(Pool);
ST(Pooled);
ST(Request);
ST(SAPI);
ST(SQL_Connection);
ST(SQL_Driver);
ST(SQL_Driver_manager);
ST(SQL_Driver_services);
ST(Stack);
ST(String);
ST(Table);
ST(Temp_alias);
ST(Temp_lang);
ST(Temp_method);
ST(VAliased);
ST(Value);
ST(VBool);
ST(VClass);
ST(VCodeFrame);
ST(VCookie);
ST(VDouble);
ST(VEnv);
ST(VFile);
ST(VForm);
ST(VHash);
ST(VImage);
ST(VInt);
ST(VJunction);
ST(VMethodFrame);
ST(VObject);
ST(VRequest);
ST(VResponse);
ST(VStateless_class);
ST(VStateless_object);
ST(VString);
ST(VTable);
ST(VVoid);
ST(WContext);
ST(WWrapper);
ST(Dictionary);

	for(int place=0; place<MALLOC_STAT_PLACES; place++)
		if(malloc_places[place]) {
			SAPI::log(pool, 
				"place:times %10d:%10d", 
				place, 
				malloc_places[place]);
			int total_size=0, total_times=0;
			for(int i=0; i<MALLOC_STAT_MAXSIZE; i++)
				if(malloc_times[place][i]) {
					total_size+=i*malloc_times[place][i];
					total_times+=malloc_times[place][i];
					SAPI::log(pool, 
						"%10d*%10d=%10d", 
						i, 
						malloc_times[place][i], 
						i*malloc_times[place][i]);
				}
			SAPI::log(pool, 
				"total %d/%d", 
				total_size, 
				total_times);
		}		
}
#endif

void *Pool::real_malloc(size_t size, int place) {
#ifdef DEBUG_POOL_MALLOC
//place=0;
	int index=min(MALLOC_STAT_MAXSIZE-1, size);
	malloc_times[place][index]++;
	malloc_places[place]++;
/*	if(size==208 && place==10)
		__asm int 3;*/
#endif
	return ::malloc(size);
}

void *Pool::real_calloc(size_t size) {
	return ::calloc(size, 1);
}

bool Pool::real_register_cleanup(void (*cleanup) (void *), void *data) {
	return fstorage!=0 && static_cast<Pool_storage *>(fstorage)->register_cleanup(cleanup, data);
}
