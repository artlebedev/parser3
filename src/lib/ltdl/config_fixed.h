/* pa_config_fixed.h. */
/* Configure file for autoconf-disabled platforms  */
/* tested only with MS Visual C++ V6 */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define as __inline if that's what the C compiler calls it.  */
#define inline __inline

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you have the libdl library or equivalent.  */
//#define HAVE_LIBDL 1

/* Define if you have the GNU dld library.  */
/* #undef HAVE_DLD */

/* Define if you have the shl_load function.  */
/* #undef HAVE_SHL_LOAD */

/* Define if you have the dlerror function.  */
//#define HAVE_DLERROR 1

/* Define if you have the index function.  */
/* #undef HAVE_INDEX */

/* Define if you have the rindex function.  */
/* #undef HAVE_RINDEX */

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the <ctype.h> header file.  */
#define HAVE_CTYPE_H 1

/* Define if you have the <dl.h> header file.  */
/* #undef HAVE_DL_H */

/* Define if you have the <dld.h> header file.  */
/* #undef HAVE_DLD_H */

/* Define if you have the <dlfcn.h> header file.  */
//#define HAVE_DLFCN_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <stdio.h> header file.  */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
/* #undef HAVE_STRINGS_H */

/* Define to the extension used for shared libraries, say, .so.  */
#define LTDL_SHLIB_EXT ".dll"

/* Define to the name of the environment variable that determines the dynamic library search path.  */
#define LTDL_SHLIBPATH_VAR "PATH"

/* Define to the sub-directory in which libtool stores uninstalled libraries.  */
#define LTDL_OBJDIR ".libs/"

/* Define if libtool can extract symbol lists from object files.  */
//#define HAVE_PRELOADED_SYMBOLS 1

/* Define if dlsym() requires a leading underscode in symbol names.  */
//#define NEED_USCORE 1

