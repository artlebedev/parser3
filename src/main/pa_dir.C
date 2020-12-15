/** @file
	Parser: directory scanning for different OS-es.

	Copyright (c) 2000-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_common.h"
#include "pa_dir.h"
#include "pa_request.h"
#include "pa_convert_utf.h"

volatile const char * IDENT_PA_DIR_C="$Id: pa_dir.C,v 1.30 2020/12/15 17:10:36 moko Exp $" IDENT_PA_DIR_H;

#ifdef _MSC_VER

const UTF16* pa_utf16_encode(const char* in, Charset& source_charset);
const char* pa_utf16_decode(const UTF16* in, Charset& asked_charset);

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

	const UTF16* utf16mask=pa_utf16_encode(mask, pa_thread_request().charsets.source());

	_ffblk->handle=FindFirstFileW((const wchar_t *)utf16mask, &_ffblk->stat);
	return _ffblk->handle==INVALID_HANDLE_VALUE;
}

bool findnext(struct ffblk *_ffblk) {
	return !FindNextFileW(_ffblk->handle, &_ffblk->stat);
}

void findclose(struct ffblk *_ffblk) {
	FindClose(_ffblk->handle);
}

const char *ffblk::name() {
	return pa_utf16_decode((const UTF16*)stat.cFileName, pa_thread_request().charsets.source());
}

bool ffblk::is_dir(bool) {
	return (stat.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

double ffblk::size() {
	ULARGE_INTEGER ull;
	ull.LowPart = stat.nFileSizeLow;
	ull.HighPart = stat.nFileSizeHigh;
	return (double)ull.QuadPart;
}

time_t ffblk::c_timestamp() {
	return filetime_to_timet(stat.ftCreationTime);
}

time_t ffblk::m_timestamp() {
	return filetime_to_timet(stat.ftLastWriteTime);
}

time_t ffblk::a_timestamp() {
	return filetime_to_timet(stat.ftLastAccessTime);
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

void ffblk::stat_file() {
	char fileSpec[MAXPATH];
	snprintf(fileSpec, MAXPATH, "%s/%s", filePath, ff_name);
	
	if(pa_stat(fileSpec, &_st) != 0) {
		memset(&_st,0,sizeof(_st));
	}
}

bool ffblk::is_dir(bool stat) {
#ifdef HAVE_STRUCT_DIRENT_D_TYPE
	if(!stat && _d_type != DT_UNKNOWN)
		return _d_type == DT_DIR;
#endif
	stat_file();
	return S_ISDIR(_st.st_mode) != 0;
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
