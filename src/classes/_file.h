/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _file.h,v 1.1 2001/03/19 20:08:11 paf Exp $
*/

#ifndef _FILE_H
#define _FILE_H

#include "pa_vstateless_class.h"

extern VStateless_class *file_base_class;
void initialize_file_base_class(Pool& pool, VStateless_class& vclass);

#endif
