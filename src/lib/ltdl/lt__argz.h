/* libltdl includes <lt__argz.h> where the system argz is missing or broken - MSVC, freebsd, macos.
   There Makefile just copies libltdl/lt__argz_.h, but MSVC has no make: include required header directly. */

#include "libltdl/lt__argz_.h"
