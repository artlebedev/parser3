/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru>

	$Id: core.h,v 1.6 2001/03/10 17:10:38 paf Exp $
*/

#ifndef CORE_H
#define CORE_H

#include "pa_string.h"

// core func
void core();
// unnamed_name helper global string
extern String *unnamed_name;
extern String *empty_string;

extern String *auto_method_name;
extern String *main_method_name;

extern String *run_class_name;
extern String *root_class_name;
extern String *env_class_name;

#endif
