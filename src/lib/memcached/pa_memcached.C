/** @file
	Parser: memcached support impl.
	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#include "pa_config_includes.h"
#include "pa_memcached.h"
#include "ltdl.h"

volatile const char * IDENT_PA_MEMCACHED_C="$Id: pa_memcached.C,v 1.11 2020/12/15 17:10:33 moko Exp $";

t_memcached f_memcached=0;
t_memcached_create f_memcached_create;
t_memcached_free f_memcached_free;
t_memcached_strerror f_memcached_strerror;

t_memcached_server_push f_memcached_server_push;
t_memcached_servers_parse f_memcached_servers_parse;
t_memcached_version f_memcached_version;

t_memcached_flush f_memcached_flush;
t_memcached_quit f_memcached_quit;

t_memcached_get f_memcached_get;
t_memcached_delete f_memcached_delete;
t_memcached_mget f_memcached_mget;
t_memcached_set f_memcached_set;
t_memcached_add f_memcached_add;

t_memcached_fetch_result f_memcached_fetch_result;
t_memcached_result_create f_memcached_result_create;
t_memcached_result_free f_memcached_result_free;

t_memcached_result_key_value f_memcached_result_key_value;
t_memcached_result_value f_memcached_result_value;
t_memcached_result_key_length f_memcached_result_key_length;
t_memcached_result_length f_memcached_result_length;
t_memcached_result_flags f_memcached_result_flags;

#define GLINK(name) f_##name=(t_##name)lt_dlsym(handle, #name);
#define DLINK(name) GLINK(name) if(!f_##name) return "function " #name " was not found";
		
void pa_dlinit();

static const char *dlink(const char *dlopen_file_spec) {
	pa_dlinit();

	lt_dlhandle handle=lt_dlopen(dlopen_file_spec);

	if(!handle){
		if(const char* result=lt_dlerror())
			return result;
		return "can not open the dynamic link module";
	}

	GLINK(memcached);
	DLINK(memcached_create);
	DLINK(memcached_free);
	DLINK(memcached_strerror);

	DLINK(memcached_server_push);
	DLINK(memcached_servers_parse);
	DLINK(memcached_version);

	DLINK(memcached_flush);
	DLINK(memcached_quit);

	DLINK(memcached_get);
	DLINK(memcached_delete);
	DLINK(memcached_mget);
	DLINK(memcached_set);
	DLINK(memcached_add);

	DLINK(memcached_fetch_result);
	DLINK(memcached_result_create);
	DLINK(memcached_result_free);

	DLINK(memcached_result_key_value);
	DLINK(memcached_result_value);
	DLINK(memcached_result_key_length);
	DLINK(memcached_result_length);
	DLINK(memcached_result_flags);

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
