/** @file
	Parser: memcached support impl.
	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_config_includes.h"
#include "pa_memcached.h"
#include "ltdl.h"

volatile const char * IDENT_PA_MEMCACHED_C="$Id: pa_memcached.C,v 1.1 2012/03/19 22:20:29 moko Exp $";

t_memcached_create f_memcached_create;
t_memcached_free f_memcached_free;
t_memcached_strerror f_memcached_strerror;
t_memcached_server_push f_memcached_server_push;
t_memcached_servers_parse f_memcached_servers_parse;
t_memcached_get f_memcached_get;
t_memcached_delete f_memcached_delete;
t_memcached_mget f_memcached_mget;
t_memcached_set f_memcached_set;

#define GLINK(name) f_##name=(t_##name)lt_dlsym(handle, #name);
#define DLINK(name) GLINK(name) if(!f_##name) return "function " #name " was not found";
		
static const char *dlink(const char *dlopen_file_spec) {
	if(lt_dlinit())
		return lt_dlerror();

	lt_dlhandle handle=lt_dlopen(dlopen_file_spec);

	if(!handle){
		if(const char* result=lt_dlerror())
			return result;
		return "can not open the dynamic link module";
	}

	DLINK(memcached_create);
	DLINK(memcached_free);
	DLINK(memcached_strerror);
	DLINK(memcached_server_push);
	DLINK(memcached_servers_parse);
	DLINK(memcached_get);
	DLINK(memcached_delete);
	DLINK(memcached_mget);
	DLINK(memcached_set);

	return 0;
}

bool memcached_linked = false;
const char *memcached_status = 0;

const char *memcached_load(const char *library_name){
	if(!memcached_linked){
		memcached_linked=true;
		memcached_status=dlink(library_name);
	}

	return memcached_status;
}
