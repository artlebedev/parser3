/** @file
	Parser: memcached support decl.
	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
*/

#ifndef PA_MEMCACHED_H
#define PA_MEMCACHED_H

#include "constants.h"
#include "types.h"

// memcached library load function
const char *memcached_load(const char *alt_library_name);

// memcached library inferface
typedef memcached_st *(*t_memcached)(const char *string, size_t string_length);
typedef memcached_st *(*t_memcached_create)(memcached_st *ptr);
typedef void (*t_memcached_free)(memcached_st *ptr);
typedef const char *(*t_memcached_strerror)(memcached_st *ptr, memcached_return_t rc);

typedef memcached_return_t (*t_memcached_server_push)(memcached_st *ptr, const memcached_server_list_st list);
typedef memcached_server_list_st (*t_memcached_servers_parse)(const char *server_strings);
typedef memcached_return_t (*t_memcached_version)(memcached_st *ptr);

typedef memcached_return_t (*t_memcached_flush)(memcached_st *ptr, time_t expiration);
typedef void (*t_memcached_quit)(memcached_st *ptr);

typedef char *(*t_memcached_get)(memcached_st *ptr, const char *key, size_t key_length, size_t *value_length, uint32_t *flags, memcached_return_t *error);
typedef memcached_return_t (*t_memcached_delete)(memcached_st *ptr, const char *key, size_t key_length, time_t expiration);
typedef memcached_return_t (*t_memcached_mget)(memcached_st *ptr, const char * const *keys, const size_t *key_length, size_t number_of_keys);
typedef memcached_return_t (*t_memcached_set)(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags);
typedef memcached_return_t (*t_memcached_add)(memcached_st *ptr, const char *key, size_t key_length, const char *value, size_t value_length, time_t expiration, uint32_t flags);

typedef memcached_result_st *(*t_memcached_fetch_result)(memcached_st *ptr, memcached_result_st *result, memcached_return_t *error);
typedef memcached_result_st *(*t_memcached_result_create)(const memcached_st *ptr, memcached_result_st *result);
typedef void (*t_memcached_result_free)(memcached_result_st *result);

typedef const char *(*t_memcached_result_key_value)(const memcached_result_st *self);
typedef const char *(*t_memcached_result_value)(const memcached_result_st *self);
typedef size_t (*t_memcached_result_key_length)(const memcached_result_st *self);
typedef size_t (*t_memcached_result_length)(const memcached_result_st *self);
typedef uint32_t (*t_memcached_result_flags)(const memcached_result_st *self);


extern t_memcached f_memcached;
extern t_memcached_create f_memcached_create;
extern t_memcached_free f_memcached_free;
extern t_memcached_strerror f_memcached_strerror;

extern t_memcached_server_push f_memcached_server_push;
extern t_memcached_servers_parse f_memcached_servers_parse;
extern t_memcached_version f_memcached_version;

extern t_memcached_flush f_memcached_flush;
extern t_memcached_quit f_memcached_quit;

extern t_memcached_get f_memcached_get;
extern t_memcached_delete f_memcached_delete;
extern t_memcached_mget f_memcached_mget;
extern t_memcached_set f_memcached_set;
extern t_memcached_add f_memcached_add;

extern t_memcached_fetch_result f_memcached_fetch_result;
extern t_memcached_result_create f_memcached_result_create;
extern t_memcached_result_free f_memcached_result_free;

extern t_memcached_result_key_value f_memcached_result_key_value;
extern t_memcached_result_value f_memcached_result_value;
extern t_memcached_result_key_length f_memcached_result_key_length;
extern t_memcached_result_length f_memcached_result_length;
extern t_memcached_result_flags f_memcached_result_flags;

#endif /* PA_MEMCACHED_H */
