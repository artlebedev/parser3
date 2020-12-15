/** @file
	Parser: directory scanning for different OS-es decls.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DIR_H
#define PA_DIR_H

#define IDENT_PA_DIR_H "$Id: pa_dir.h,v 1.30 2020/12/15 17:10:31 moko Exp $"

#include "pa_config_includes.h"

/** @struct ffblk 
	win32/unix unified directory entry structure name
	for findfirst/next/close interface
*/

#ifdef _MSC_VER

#include <windows.h>

#define MAXPATH MAX_PATH

struct ffblk {
	struct _WIN32_FIND_DATAW stat;
	HANDLE handle;

	const char *name();
	bool is_dir(bool);
	double size();
	time_t c_timestamp();
	time_t m_timestamp();
	time_t a_timestamp();
};

#else

#define MAXPATH 1000 /*NAME_MAX*/

struct ffblk {
	/*as if in windows :)*/
	char ff_name[ MAXPATH ];
	/*helpers*/
	DIR *dir;
	const char *filePath;
	struct stat _st;

#ifdef HAVE_STRUCT_DIRENT_D_TYPE
	unsigned char _d_type;
#endif

	const char *name(){ return ff_name; }
	bool is_dir(bool);
	void stat_file();
	double size();
	time_t c_timestamp();
	time_t m_timestamp();
	time_t a_timestamp();
};

#endif

bool findfirst(const char* _pathname, struct ffblk *_ffblk, int _attrib);
bool findnext(struct ffblk *_ffblk);
void findclose(struct ffblk *_ffblk);

/// main dir workhorse: calles win32/unix unified functions findfirst/next/close [skip . and ..]
#define LOAD_DIR(dir,action) { \
	ffblk ffblk; \
	if(!findfirst(dir, &ffblk, 0)) { \
		do { \
			const char *file_name=ffblk.name(); \
			if(*file_name && !(file_name[0]=='.' && (file_name[1]==0 || file_name[1]=='.' && file_name[2]==0)  )) { \
				action; \
			} \
		} while(!findnext(&ffblk)); \
		findclose(&ffblk); \
	} \
}

#endif
