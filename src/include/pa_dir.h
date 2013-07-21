/** @file
	Parser: directory scanning for different OS-es decls.

	Copyright (c) 2000-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DIR_H
#define PA_DIR_H

#define IDENT_PA_DIR_H "$Id: pa_dir.h,v 1.23 2013/07/21 20:33:44 moko Exp $"

#include "pa_config_includes.h"

/** @struct ffblk 
	win32/unix unified directory entry structure name
	for findfirst/next/close interface
*/

#ifdef WIN32

#define MAXPATH MAX_PATH

struct ffblk {
	DWORD ff_attrib;/*dwFileAttributes;*/
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	DWORD nFileSizeHigh;
	DWORD nFileSizeLow;
	DWORD dwReserved0;
	DWORD dwReserved1;
	CHAR   ff_name[ MAX_PATH ];/*cFileName[ MAX_PATH ];*/
	CHAR   cAlternateFileName[ 14 ];
	/*helper*/
	HANDLE handle;

	void stat_file(){}
	bool is_dir();
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
	void stat_file();
#else
	void stat_file(){}
	void real_stat_file();
#endif
	bool is_dir();
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
#define LOAD_DIR(dir,action) {\
    ffblk ffblk; \
    if(!findfirst(dir, &ffblk, 0)) { \
		do \
			if(*ffblk.ff_name && !(ffblk.ff_name[0]=='.' && (ffblk.ff_name[1]==0 || ffblk.ff_name[1]=='.' && ffblk.ff_name[2]==0)  )) {\
				action; \
			} \
		while(!findnext(&ffblk)); \
		findclose(&ffblk); \
	} \
} 

#endif
