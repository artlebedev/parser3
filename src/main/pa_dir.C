/** @file
	Parser: directory scanning for different OS-es.

	Copyright (c) 2000,2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_dir.C,v 1.3 2001/05/17 19:33:33 parser Exp $
*/

#include "pa_dir.h"
#include "pa_common.h"

#ifdef WIN32

bool findfirst(const char *_pathname, struct ffblk *_ffblk, int _attrib) {
	char mask[MAXPATH];
	snprintf(mask, MAXPATH, "%s/*.*", _pathname);

	_ffblk->handle=FindFirstFile(mask, (_WIN32_FIND_DATAA *)_ffblk);
	return _ffblk->handle==INVALID_HANDLE_VALUE;
}

bool findnext(struct ffblk *_ffblk) {
	return !FindNextFile(_ffblk->handle, (_WIN32_FIND_DATAA *)_ffblk);}

void findclose(struct ffblk *_ffblk) {
	FindClose(_ffblk->handle);
}

#else

bool findfirst(const char *_pathname, struct ffblk *_ffblk, int _attrib) {
    strncpy(_ffblk->filePath, _pathname, MAXPATH);
	if(!(_ffblk->dir=opendir(_ffblk->filePath)))
        return true;

	return findnext(_ffblk);
}

bool findnext(struct ffblk *_ffblk) {
    while(true) {
        struct dirent *entry=readdir(_ffblk->dir);
        if(!entry)
            return true;

		int maxsize=sizeof(_ffblk->ff_name)-1;
		strncpy(_ffblk->ff_name, entry->d_name, maxsize);
		_ffblk->ff_name[maxsize]=0;
		
		char fileSpec[MAXPATH];
		snprintf(fileSpec, MAXPATH, "%s/%s",
			_ffblk->filePath,
			_ffblk->ff_name);
		
		struct stat st;
		
		_ffblk->ff_attrib =
			stat(fileSpec, &st) < 0 ? 0/*would fail later*/ : st.st_mode;
		return false;
    }
}

void findclose(struct ffblk *_ffblk) {
	closedir(_ffblk->dir);
}

#endif
