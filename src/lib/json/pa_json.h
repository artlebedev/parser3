/*
 * Copyright (C) 2009-2011 Vincent Hanquez <vincent@snarc.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 or version 3.0 only.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef JSON_H
#define JSON_H

#include "pa_config_includes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum 
{
	JSON_NONE,
	JSON_ARRAY_BEGIN,
	JSON_OBJECT_BEGIN,
	JSON_ARRAY_END,
	JSON_OBJECT_END,
	JSON_INT,
	JSON_FLOAT,
	JSON_STRING,
	JSON_KEY,
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL
} json_type;

typedef enum
{
	/* SUCCESS = 0 */
	/* running out of memory */
	JSON_ERROR_NO_MEMORY = 1,
	/* character < 32, except space newline tab */
	JSON_ERROR_BAD_CHAR,
	/* trying to pop more object/array than pushed on the stack */
	JSON_ERROR_POP_EMPTY,
	/* trying to pop wrong type of mode. popping array in object mode, vice versa */
	JSON_ERROR_POP_UNEXPECTED_MODE,
	/* reach nesting limit on stack */
	JSON_ERROR_NESTING_LIMIT,
	/* reach data limit on buffer */
	JSON_ERROR_DATA_LIMIT,
	/* comment are not allowed with current configuration */
	JSON_ERROR_COMMENT_NOT_ALLOWED,
	/* unexpected char in the current parser context */
	JSON_ERROR_UNEXPECTED_CHAR,
	/* unicode low surrogate missing after high surrogate */
	JSON_ERROR_UNICODE_MISSING_LOW_SURROGATE,
	/* unicode low surrogate missing without previous high surrogate */
	JSON_ERROR_UNICODE_UNEXPECTED_LOW_SURROGATE,
	/* found a comma not in structure (array/object) */
	JSON_ERROR_COMMA_OUT_OF_STRUCTURE,
	/* callback returns error */
	JSON_ERROR_CALLBACK
} json_error;

#define LIBJSON_DEFAULT_STACK_SIZE 256
#define LIBJSON_DEFAULT_BUFFER_SIZE 4096

typedef int (*json_parser_callback)(void *userdata, int type, const char *data, uint32_t length);

typedef struct {
	uint32_t buffer_initial_size;
	uint32_t max_nesting;
	uint32_t max_data;
	int allow_c_comments;
	int allow_yaml_comments;
	void * (*user_malloc)(size_t size);
	void * (*user_realloc)(void *ptr, size_t size);
	void (*user_free)(void *ptr);
} json_config;

typedef struct json_parser {
	json_config config;

	/* SAJ callback */
	json_parser_callback callback;
	void *userdata;

	/* parser state */
	uint8_t state;
	uint8_t save_state;
	uint8_t expecting_key;
	uint16_t unicode_multi;
	json_type type;

	/* state stack */
	uint8_t *stack;
	uint32_t stack_offset;
	uint32_t stack_size;

	/* parse buffer */
	char *buffer;
	uint32_t buffer_size;
	uint32_t buffer_offset;
} json_parser;

/** json_parser_init initialize a parser structure taking a config,
 * a config and its userdata.
 * return JSON_ERROR_NO_MEMORY if memory allocation failed or SUCCESS.  */
int json_parser_init(json_parser *parser, json_config *cfg,
                     json_parser_callback callback, void *userdata);

/** json_parser_free freed memory structure allocated by the parser */
int json_parser_free(json_parser *parser);

/** json_parser_string append a string s with a specific length to the parser
 * return 0 if everything went ok, a JSON_ERROR_* otherwise.
 * the user can supplied a valid processed pointer that will
 * be fill with the number of processed characters before returning */
int json_parser_string(json_parser *parser, const char *string,
                       uint32_t length, uint32_t *processed);

/** json_parser_char append one single char to the parser
 * return 0 if everything went ok, a JSON_ERROR_* otherwise */
int json_parser_char(json_parser *parser, unsigned char next_char);

/** json_parser_is_done return 0 is the parser isn't in a finish state. !0 if it is */
int json_parser_is_done(json_parser *parser);

#ifdef __cplusplus
}
#endif

#endif /* JSON_H */
