/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: core.h,v 1.11 2001/03/12 12:00:04 paf Exp $
*/

#ifndef CORE_H
#define CORE_H

#include "pa_string.h"
#include "pa_hash.h"

// core func
void core();

// global strings
extern String *unnamed_name;
extern String *empty_string;

extern String *auto_method_name;
extern String *main_method_name;

extern String *main_class_name;
extern String *root_class_name;
extern String *env_class_name;
extern String *table_class_name;

// global hashes
extern Hash *untaint_lang_name2enum;

#endif
