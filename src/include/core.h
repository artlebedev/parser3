/*
  $Id: core.h,v 1.4 2001/03/10 15:56:14 paf Exp $
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

extern String *auto_class_name;
extern String *run_class_name;
extern String *root_class_name;
extern String *env_class_name;

#endif
