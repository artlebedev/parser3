/** @file
	Parser: program executing for different OS-es decls.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_EXEC_H
#define PA_EXEC_H

#define IDENT_PA_EXEC_H "$Id: pa_exec.h,v 1.24 2020/12/15 17:10:31 moko Exp $"

#include "pa_string.h"
#include "pa_hash.h"
#include "pa_array.h"
#include "pa_value.h"
#include "pa_charset.h"

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
