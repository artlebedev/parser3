/*
	Parser
	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: _file.h,v 1.2 2001/03/19 21:39:30 paf Exp $
*/

#ifndef _FILE_H
#define _FILE_H

#include "pa_vstateless_class.h"

extern VStateless_class *file_class;
void initialize_file_class(Pool& pool, VStateless_class& vclass);

#endif
