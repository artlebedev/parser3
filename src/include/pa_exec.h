/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000,2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

static const char* IDENT_EXEC_H="$Id: pa_exec.h,v 1.9 2002/08/01 11:26:47 paf Exp $";

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"

/// @return exit code
int pa_exec(
			bool forced_allow, ///< allow execution regardles of --disable-execs
			const String& file_spec, 
			const Hash *env, ///< 0 | Hash of String
			const Array *argv, ///< 0 | Array of command line arguments
			const String& in, String& out, String& err);

#endif
