/* pa_config_fixed.h. */
/* Configure file for autoconf-disabled platforms  */
/* tested only with MS Visual C++ V6 */

#define HAVE_CTYPE_H 1
#define HAVE_ERRNO_H 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMORY_H 1
#  include <memory.h>
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRCHR 1
#define HAVE_STRCMP 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_STRRCHR 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define LTDL_OBJDIR ".libs/"
#define LTDL_SHLIB_EXT ".dll"
/*
	MS is shy to admit it implements stdc when compiled with lang extensions
	without lang extensions libltdl does not compile
*/
#define __STDC__ 1
#define error_t int
#define inline __inline

#define R_OK 4

/* dirent.h replacement  */
#include <windows.h>
#include <io.h>
#define access(name, flags) _access(name, flags)
struct dirent { 
		char *d_name; 
};
typedef void DIR;

#define LT_D_NAMLEN(dirent) (0)

inline DIR              *opendir(const char *name) {
	return 0;
}
inline struct dirent    *readdir(DIR *dir) {
	return 0;
}
inline int              closedir(DIR *dir) {
	return 0;
}