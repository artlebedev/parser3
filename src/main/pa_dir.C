/** @file
	Parser: directory scanning for different OS-es.

	Copyright (c) 2000-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_dir.h"

volatile const char * IDENT_PA_DIR_C="$Id: pa_dir.C,v 1.24 2013/07/22 20:55:54 moko Exp $" IDENT_PA_DIR_H;

#ifdef _MSC_VER

#define TICKS_PER_SECOND 10000000ULL
#define EPOCH_DIFFERENCE 11644473600ULL
time_t filetime_to_timet(FILETIME const& ft){
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	long long secs=(ull.QuadPart / TICKS_PER_SECOND - EPOCH_DIFFERENCE);
	time_t t =(time_t)secs;
	return (secs == (long long)t) ? t : 0;
}

bool findfirst(const char* _pathname, struct ffblk *_ffblk, int /*_attrib*/) {
	char mask[MAXPATH];
	snprintf(mask, MAXPATH, "%s/*.*", _pathname);

	_ffblk->handle=FindFirstFile(mask, (_WIN32_FIND_DATAA *)_ffblk);
	return _ffblk->handle==INVALID_HANDLE_VALUE;
}

bool findnext(struct ffblk *_ffblk) {
	return !FindNextFile(_ffblk->handle, (_WIN32_FIND_DATAA *)_ffblk);
}

void findclose(struct ffblk *_ffblk) {
	FindClose(_ffblk->handle);
}

bool ffblk::is_dir() {
	return (ff_attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

double ffblk::size() {
	ULARGE_INTEGER ull;
	ull.LowPart = nFileSizeLow;
	ull.HighPart = nFileSizeHigh;
	return (double)ull.QuadPart;
}

time_t ffblk::c_timestamp() {
	return filetime_to_timet(ftCreationTime);
}

time_t ffblk::m_timestamp() {
	return filetime_to_timet(ftLastWriteTime);
}

time_t ffblk::a_timestamp() {
	return filetime_to_timet(ftLastAccessTime);
}

#else

bool findfirst(const char* _pathname, struct ffblk *_ffblk, int /*_attrib*/) {
	_ffblk->filePath=_pathname;
	if(!(_ffblk->dir=opendir(_ffblk->filePath)))
        return true;

	return findnext(_ffblk);
}

bool findnext(struct ffblk *_ffblk) {
    while(true) {
        struct dirent *entry=readdir(_ffblk->dir);
        if(!entry)
            return true;

		strncpy(_ffblk->ff_name, entry->d_name, sizeof(_ffblk->ff_name)-1);
		_ffblk->ff_name[sizeof(_ffblk->ff_name)-1]=0;
		
#ifdef HAVE_STRUCT_DIRENT_D_TYPE
		// http://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
		_ffblk->_d_type=entry->d_type;
#endif
		return false;
    }
}

void findclose(struct ffblk *_ffblk) {
	closedir(_ffblk->dir);
}

#ifdef HAVE_STRUCT_DIRENT_D_TYPE
void ffblk::stat_file() {
#else
void ffblk::real_stat_file() {
#endif
	char fileSpec[MAXPATH];
	snprintf(fileSpec, MAXPATH, "%s/%s", filePath, ff_name);
	
	if(stat(fileSpec, &_st) != 0) {
		memset(&_st,0,sizeof(_st));
	}
}

bool ffblk::is_dir() {
#ifdef HAVE_STRUCT_DIRENT_D_TYPE
	return (_d_type & DT_DIR) != 0;
#else
	real_stat_file();
	return S_ISDIR(_st.st_mode) != 0;
#endif
}

double ffblk::size() {
	return (double)_st.st_size;
}

time_t ffblk::c_timestamp() {
	return _st.st_ctime;
}

time_t ffblk::m_timestamp() {
	return _st.st_mtime;
}

time_t ffblk::a_timestamp() {
	return _st.st_atime;
}

#endif
