/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _env.h,v 1.5 2001/03/13 16:38:21 paf Exp $
*/

#ifndef _ENV_H
#define _ENV_H

#include "pa_vstateless_class.h"

extern VStateless_class *env_base_class;
void initialize_env_base_class(Pool& pool, VStateless_class& vclass);

#endif
