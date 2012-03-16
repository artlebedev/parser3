/** @file
	Parser: directory scanning for different OS-es decls.

	Copyright (c) 2000-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_DIR_H
#define PA_DIR_H

#define IDENT_PA_DIR_H "$Id: pa_dir.h,v 1.20 2012/03/16 09:24:09 moko Exp $"

#include "pa_config_includes.h"

/** @struct ffblk 
	win32/unix unified directory entry structure name
	for findfirst/next/close interface
*/

#ifdef WIN32

#define  WINVER  0x0400
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
