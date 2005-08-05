/** @file
	Parser: directory scanning for different OS-es decls.

	Copyright (c) 2000,2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DIR_H
#define PA_DIR_H

static const char * const IDENT_DIR_H="$Date: 2005/08/05 13:02:59 $";

#include "pa_config_includes.h"

/** @struct ffblk 
	win32/unix unified directory entry structure name
	for findfirst/next/close interface
*/

#ifdef WIN32

#include <windows.h>

#define MAXPATH MAX_PATH
#define FA_DIREC FILE_ATTRIBUTE_DIRECTORY

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

/*	
	
	unsigned char  ff_attrib __attribute__((packed));
  unsigned short ff_ftime __attribute__((packed));
  unsigned short ff_fdate __attribute__((packed));
  unsigned long  ff_fsize __attribute__((packed));
  char ff_name[260] __attribute__((packed));
 */
};

#else

#include <strings.h>	
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAXPATH 1000 /*NAME_MAX*/
#define FA_DIREC S_IFDIR

struct ffblk {
	/*as if in windows :)*/
    unsigned char ff_attrib;
    char ff_name[ MAXPATH ];
	/*helpers*/
	DIR *dir;
	char filePath[MAXPATH];
};

#endif

bool findfirst(const char* _pathname, struct ffblk *_ffblk, int _attrib);
bool findnext(struct ffblk *_ffblk);
void findclose(struct ffblk *_ffblk);

/// main dir workhorse: calles win32/unix unified functions findfirst/next/close
#define LOAD_DIR(dir,action) {\
    ffblk ffblk; \
    if(!findfirst(dir, &ffblk, 0)) { \
		do \
			if(*ffblk.ff_name && ffblk.ff_name[0]!='.') {\
				action; \
			} \
		while(!findnext(&ffblk)); \
		findclose(&ffblk); \
	} \
} 

#endif
