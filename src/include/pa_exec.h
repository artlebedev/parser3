/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

#define IDENT_PA_EXEC_H "$Id: pa_exec.h,v 1.29 2026/10/28 20:22:22 moko Exp $"

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"
#include "pa_value.h"
#include "pa_charset.h"
#include "pa_file.h"

/// pid of the active exec child, 0 if none; also its process group id: killed on the response timeout
extern THREAD_LOCAL int pa_exec_active_pid;

struct PA_exec_result {
	int status; ///< exit code
	File_read_result out;
	String& err;

	PA_exec_result(): status(0), out(), err(*new String) {}
};

PA_exec_result pa_exec(
			bool forced_allow, ///< allow execution regardles of --disable-execs
			const String& file_spec, 
			const HashStringString* env, ///< 0 | Hash of String
			const ArrayString& argv, ///< 0 | Array of command line arguments
			String::C in);

#endif
