/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000,2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_exec.h,v 1.2 2001/05/17 10:22:24 parser Exp $
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

#include "pa_config_includes.h"
#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"

/// @return exit code
int pa_exec(const String& file_spec, 
			const Hash *env, ///< 0 | Hash of String
			const Array *argv, ///< 0 | Array of command line arguments
			const String& in, String& out, String& err);

#endif
