/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000,2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

  	portions by Victor Fedoseev" <vvf_ru@mail.ru> [January 23, 2003]
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

static const char* IDENT_EXEC_H="$Date: 2003/04/04 14:42:38 $";

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"

/// @return exit code
int pa_exec(
			bool forced_allow, ///< allow execution regardles of --disable-execs
			const String& file_spec, 
			const Hash *env, ///< 0 | Hash of String
			const Array *argv, ///< 0 | Array of command line arguments
			const String& in, String& out, String& err,
			int *header_pos=0, const char **eol_marker=0, size_t *eol_marker_size=0);

#endif
