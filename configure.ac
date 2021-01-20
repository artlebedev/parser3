dnl Autoconf initialization
AC_PREREQ(2.59)
AC_INIT(parser, 3.4.6)
AC_CONFIG_SRCDIR(README)


dnl Automake Initialization
AM_INIT_AUTOMAKE


dnl Expand srcdir
P3S=`cd $srcdir/src ; pwd`
AC_SUBST(P3S)


dnl Parser version update
AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(PARSER_VERSION,"$VERSION (compiled on $host)",parser version)

AC_SUBST(host_os)
case $host_os in
  *cygwin* ) AC_DEFINE(CYGWIN,,using cygwin building environment);;
esac


dnl Checks for programs
AC_PROG_INSTALL
AC_PROG_AWK

AC_PROG_YACC
if test "$YACC" != "bison -y"; then
	AC_MSG_WARN(to regenerate Parser grammar YOU WOULD NEED BISON)
else
	AC_MSG_CHECKING(bison version)
	oldIFS=$IFS; IFS=.
	set `bison -V | sed -e 's/^GNU Bison version //' -e 's/^bison (GNU Bison) //' -e 's/$/./'`
	IFS=$oldIFS
	if test "$1" = "1" -a "$2" -lt "25"; then
		AC_MSG_WARN(Bison 1.25 or newer needed to regenerate Parser compiler (found $1.$2).)
	fi
	AC_MSG_RESULT($1.$2 (ok))
fi

AC_PROG_CXX
AC_PROG_CC

dnl most tests should be compiled with C compiler [especially qsort test]
AC_LANG_C


dnl Dll extension
AC_MSG_CHECKING(for dynamic-link library extension)
case "$host_os" in
  *cygwin* ) dll_extension=dll;;
  * ) dll_extension=so
esac
AC_MSG_RESULT($dll_extension)
AC_SUBST(dll_extension)


dnl Misc arguments
AC_ARG_WITH(build-warnings, [  --with-build-warnings   to enable build-time compiler warnings if gcc is used],
	AC_MSG_WARN(enabling compiler warnings)
	CXXFLAGS="$CXXFLAGS -Wextra -Wall -Wno-parentheses"
)

AC_ARG_WITH(coverage, [  --with-coverage         to enable gcov code coverage support],
	AC_MSG_WARN(enabling gcov support)
	CFLAGS=`echo "$CFLAGS" | $SED -e 's/-O[0-9]*//g'`
	CXXFLAGS=`echo "$XXCFLAGS" | $SED -e 's/-O[0-9]*//g'`
	CFLAGS="$CFLAGS -O0 -fprofile-arcs -ftest-coverage"
	CXXFLAGS="$CXXFLAGS -O0 -fprofile-arcs -ftest-coverage"
	LDFLAGS="$LDFLAGS -lgcov"
)

AC_ARG_WITH(profiler, [  --with-profiler         to enable gprof code profiler support],
	AC_MSG_WARN(enabling gprof support)
	CFLAGS="$CFLAGS -O0 -pg -g"
	CXXFLAGS="$CXXFLAGS -O0 -pg -g"
)

AC_ARG_WITH(assertions, [  --with-assertions       to enable assertions],
	AC_MSG_WARN(enabling assertions)
,
	AC_DEFINE(NDEBUG,,assertions disabled)
)

AC_ARG_WITH(sjlj-exceptions, [  --with-sjlj-exceptions  to enable simple 'throw' from dynamic library],
	AC_DEFINE(PA_WITH_SJLJ_EXCEPTIONS,,one can throw from dynamic library)
)


AC_ARG_WITH(system-cfg, [  --with-system-cfg=FILE  to specify system-wide auto.p],
	AC_DEFINE_UNQUOTED(SYSTEM_CONFIG_FILE,"$withval",system-wide auto.p)
)


dnl Safe mode argument
AC_ARG_ENABLE(safe-mode, [  --disable-safe-mode	  to enable reading and executing
                          files belonging to group+user other then effective],
[
	SAFE_MODE=$enableval
]
)

if test "$SAFE_MODE" = "no"; then
	AC_MSG_WARN(enabling reading of files belonging to group+user other then effective)
else
	AC_DEFINE(PA_SAFE_MODE,,disabled reading of files belonging to group+user other then effective)
fi


dnl Disable execs argument
AC_ARG_ENABLE(execs, [  --disable-execs	  to disable any execs
                          (file::exec, file::cgi, unix mail:send)],
[
if test "$enableval" = "no"; then
	AC_MSG_WARN(disabling file execs)
	AC_DEFINE(NO_PA_EXECS,,pa_exec disabled)
fi
]
)


dnl String stream argument
AC_ARG_ENABLE(stringstream, [  --disable-stringstream to disable stringstream usage.
                          when disabled table.save uses more memory, but more compilers are supported],
[
if test "$enableval" = "no"; then
	AC_MSG_WARN(disabling stringstream usage)
	AC_DEFINE(NO_STRINGSTREAM,,stringstream disabled)
fi
]
)


dnl GC argument
AC_ARG_WITH(gc,[  --with-gc[=DIR]           DIR is the directory where
                          Boehm garbage collecting library is installed],[

	GC=$withval
	GC_LIBS="$GC/libgc.la"

	if test -f $GC_LIBS; then
		GC_OK="yes"
	else
		GC_LIBS="-L$GC -lgc"
	fi

	if test "$GC" = "yes"; then
		GC=""
		GC_LIBS="-lgc"
		AC_MSG_WARN([--with-gc value was not specified, hoping linker would find it])
	fi
],[
	GC_LIBS="-lgc"
	AC_MSG_WARN([--with-gc was not specified, hoping linker would find it])
])

if test -z "$GC_OK"; then
	AC_MSG_CHECKING(for libgc)
	SAVE_LIBS=$LIBS
	LIBS="$LIBS $GC_LIBS"
	AC_TRY_LINK([ extern int GC_dont_gc; ],[ GC_dont_gc=0; ],
		AC_MSG_RESULT(yes)
	,
		AC_MSG_RESULT(no)
		if test -z "$GC"; then
			AC_MSG_ERROR(please specify path to libgc: --with-gc=DIR)
		else
			AC_MSG_ERROR($GC does not seem to be valid libgc installation directory)
		fi
	)
	LIBS=$SAVE_LIBS
fi

AC_SUBST(GC_LIBS)


dnl PCRE argument
AC_ARG_WITH(pcre,[  --with-pcre=DIR         DIR is the directory where
                          PCRE library is installed],[
	PCRE=$withval
	PCRE_INCLUDES="-I$PCRE/include"
	PCRE_LIBS="$PCRE/lib/libpcre.la"

	if test -f $PCRE/include/pcre.h -a -f $PCRE_LIBS; then
		PCRE_OK="yes"
	else
		PCRE_LIBS="-L$PCRE/lib -lpcre"
	fi

	if test "$PCRE" = "yes"; then
		PCRE=""
		PCRE_LIBS="-lpcre"
		PCRE_INCLUDES=""
		AC_MSG_WARN([--with-pcre value was not specified, hoping linker would find it])
	fi
],[
	PCRE_LIBS="-lpcre"
	PCRE_INCLUDES=""
	AC_MSG_WARN([--with-pcre was not specified, hoping linker would find it])
])

if test -z "$PCRE_OK"; then
	AC_MSG_CHECKING(for prce)
	SAVE_LIBS=$LIBS
	LIBS="$LIBS $PCRE_LIBS $PCRE_INCLUDES"
	AC_TRY_LINK([ #include <pcre.h> ],[ const char *v=pcre_version(); ],
		AC_MSG_RESULT(yes)
	,
		AC_MSG_RESULT(no)
		if test -z "$PCRE"; then
			AC_MSG_ERROR(please specify path to PCRE: --with-pcre=DIR)
		else
			AC_MSG_ERROR($PCRE does not seem to be valid PCRE installation directory)
		fi
	)
	LIBS=$SAVE_LIBS
fi

AC_SUBST(PCRE_INCLUDES)
AC_SUBST(PCRE_LIBS)


dnl XML/XSLT argument
AC_ARG_WITH(xml,[  --with-xml=DIR          DIR is the directory where
                          Gnome XML libraries are installed],[

	XML=$withval
	XML_LIBS="-lxml2 -lxslt -lexslt"

	if test -z "$XML" -o "$XML" = "yes"; then
		XML=""
		XML_INCLUDES="-I/usr/include/libxml2"
		AC_MSG_WARN([--with-xml value was not specified, hoping linker would find it])
	else
		XML_INCLUDES="-I$XML/include -I$XML/include/libxml2"
		XML_LIBS="-L$XML/lib $XML_LIBS"

		if test -f $XML/include/libxslt/xslt.h -a -f $XML/lib/libxml2.la \
			-a -f $XML/lib/libxslt.la -a -f $XML/lib/libexslt.la; then
			XML_LIBS="$XML/lib/libxml2.la $XML/lib/libxslt.la $XML/lib/libexslt.la"
			XML_OK="yes"
		fi
	fi

	if test -z "$XML_OK"; then
		AC_MSG_CHECKING(for xml)
		SAVE_LIBS=$LIBS
		LIBS="$LIBS $XML_LIBS $XML_INCLUDES"
		AC_TRY_LINK([ #include <libxslt/xslt.h> ],[ const char *v=xsltEngineVersion; ],
			AC_MSG_RESULT(yes)
		,
			AC_MSG_RESULT(no)
			if test -z "$XML"; then
				AC_MSG_ERROR(please specify path to Gnome XML libraries: --with-xml=DIR)
			else
				AC_MSG_ERROR($XML does not seem to be valid Gnome XML installation directory)
			fi
		)
		LIBS=$SAVE_LIBS
	fi
	AC_DEFINE(XML,,xml-abled parser)
])

AC_SUBST(XML_INCLUDES)
AC_SUBST(XML_LIBS)


dnl Mail receive argument
AC_ARG_WITH(mailreceive,[  --with-mailreceive=DIR  is the directory where
                          Gnome MIME library is installed],[
	MIME=$withval
	GLIB="glib-2.0"
	GMIME="gmime-2.4"

	if test -z "$MIME" -o "$MIME" = "yes"; then
		MIME=""
		MIME_INCLUDES=`pkg-config --cflags $GMIME 2>/dev/null`
		MIME_LIBS=`pkg-config --libs $GMIME 2>/dev/null`
		if test -z "$MIME_LIBS"; then
			GMIME="gmime-3.0"
			MIME_INCLUDES=`pkg-config --cflags $GMIME 2>/dev/null`
			MIME_LIBS=`pkg-config --libs $GMIME 2>/dev/null`
		fi
		AC_MSG_WARN([--with-mailreceive value was not specified, hoping linker would find Gnome MIME library])
	else
		MIME_INCLUDES="-I$MIME/include/$GMIME"
		MIME_LIBS="-l$GMIME"
		if test -f $MIME/include/$GMIME/gmime/gmime.h -a -f $MIME/lib/lib$GMIME.la; then
			MIME_LIBS="$MIME/lib/lib$GMIME.la"
			if test -f $MIME/lib/lib$GLIB.la; then
				MIME_INCLUDES="$MIME_INCLUDES -I$MIME/include/$GLIB -I$MIME/lib/$GLIB/include"
			else
				GLIB_INCLUDES=`pkg-config --cflags $GLIB 2>/dev/null`
				MIME_INCLUDES="$MIME_INCLUDES $GLIB_INCLUDES"
			fi
			MIME_OK="yes"
		fi
	fi

	if test -z "$MIME_OK"; then
		AC_MSG_CHECKING(for mime)
		SAVE_LIBS=$LIBS
		LIBS="$LIBS $MIME_LIBS $MIME_INCLUDES"
		AC_TRY_LINK([ #include <gmime/gmime.h> ],[ guint v=gmime_major_version; ],
			AC_MSG_RESULT(yes)
		,
			AC_MSG_RESULT(no)
			if test -z "$MIME"; then
				AC_MSG_ERROR(please specify path to Gnome MIME library: --with-mailreceive=DIR)
			else
				AC_MSG_ERROR($MIME does not seem to be valid Gnome MIME installation directory)
			fi
		)
		LIBS=$SAVE_LIBS
	fi
	AC_DEFINE(WITH_MAILRECEIVE,,has \$mail:received)
])

AC_SUBST(MIME_INCLUDES)
AC_SUBST(MIME_LIBS)


dnl Sendmail argument
AC_ARG_WITH(sendmail,[  \"--with-sendmail=COMMAND\" forces this command to send mail.
                          example: \"--with-sendmail=/usr/sbin/sendmail -t\"
                          (makes parser ignore user-defined sendmail commands)],
	AC_DEFINE_UNQUOTED(PA_FORCED_SENDMAIL,"$withval",parser uses this command instead of user-defined sendmail commands)
)


dnl Apache module argument
AC_ARG_WITH(apache,[  --with-apache=FILE      FILE is the full path for APXS
                          builds apache DSO module using apxs],[
	APXS=$withval

	if test -z "$APXS" -o "$APXS" = "yes"; then
		APXS=`which apxs 2>/dev/null`
		if test -z "$APXS"; then
			APXS=`which apxs2 2>/dev/null`
		fi
	fi

	APACHE=`$APXS -q TARGET 2>/dev/null`

	if test -z "$APACHE"; then
		AC_MSG_ERROR($APXS does not seem to be valid apache apxs utility path)
	fi

	APACHE_MAIN_INC=`$APXS -q INCLUDEDIR`
	APACHE_EXTRA_INC=`$APXS -q EXTRA_INCLUDES 2>/dev/null`
	APACHE_INC="-I$APACHE_MAIN_INC $APACHE_EXTRA_INC"
	APACHE_CFLAGS=`$APXS -q CFLAGS`
])
AC_SUBST(APACHE)
AC_SUBST(APACHE_INC)
AC_SUBST(APACHE_CFLAGS)
AM_CONDITIONAL(COMPILE_APACHE_MODULE, test -n "$APACHE")


dnl Enable building of the convenience library
LT_CONFIG_LTDL_DIR(src/lib/ltdl)
LT_INIT(dlopen win32-dll no-pic)
LTDL_INIT


dnl Checks for typedefs, structures, and compiler characteristics
AC_C_BIGENDIAN(
	AC_DEFINE(PA_BIG_ENDIAN,,compile for sparc processor)
,
	AC_DEFINE(PA_LITTLE_ENDIAN,,compile for intel processor or compatible)
,
	AC_MSG_ERROR(word endianness not determined for some obscure reason)
)

AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

AC_STRUCT_DIRENT_D_TYPE

AC_SYS_LARGEFILE

dnl Checks for C header files
AC_HEADER_STDC
AC_HEADER_TIME

AC_CHECK_HEADERS(stdio.h sys/types.h sys/stat.h stdlib.h stddef.h memory.h string.h strings.h inttypes.h stdint.h unistd.h)
AC_CHECK_HEADERS(assert.h limits.h ctype.h math.h process.h stdarg.h setjmp.h signal.h pthread.h)
AC_CHECK_HEADERS(errno.h dirent.h fcntl.h io.h sys/file.h sys/locking.h sys/select.h sys/resource.h sys/wait.h)
AC_CHECK_HEADERS(sys/socket.h netinet/in.h arpa/inet.h netdb.h)


dnl Checks for libraries
case "$host" in
  *-freebsd4*)
	  AC_DEFINE(FREEBSD4,,FreeBSD4.X target platform)
  ;;
  *-freebsd1*)
	  AC_DEFINE(FREEBSD1X,,FreeBSD1X.X target platform)
  ;;
  *-sunos5.6* | *-solaris2.6*)
	  AC_CHECK_LIB(xnet, main)
  ;;
  *-sunos5* | *-solaris2*)
	  AC_CHECK_LIB(socket, main)
	  AC_CHECK_LIB(nsl, main)
  ;;
  *-nec-sysv4*)
	  AC_CHECK_LIB(nsl, gethostbyname)
	  AC_CHECK_LIB(socket, socket)
  ;;
  *-cygwin*)
	  AC_DEFINE(WIN32,,Windows32 target platform)
  ;;
esac

AC_CHECK_LIB(m, sin)
AC_CHECK_LIB(crypt, crypt)


dnl Checks for functions
AC_CHECK_FUNCS(flock _locking fcntl lockf ftruncate fchmod)
AC_CHECK_FUNCS(getrusage gettimeofday crypt sigsetjmp siglongjmp unsetenv)


dnl on some linux[seen on 2.4] it's a macro
PA_CHECK_SIGSETJMP


dnl see comment above
AC_LANG_PUSH(C++)
PA_CHECK_MATH_FUNCS_ONE_ARG(trunc round sign isfinite)
AC_LANG_POP


dnl We require qsort(3)
AC_CHECK_FUNCS(qsort, , AC_MSG_ERROR([No qsort library function.]))


dnl For build-in date implementation we need to know local offset from GMT

AC_LANG_PUSH(C++)
AC_MSG_CHECKING(for timezone variable)
AC_TRY_COMPILE([#include <time.h>], [int test=timezone;], AC_DEFINE(HAVE_TIMEZONE) AC_MSG_RESULT(yes), AC_MSG_RESULT(no))
AC_LANG_POP

AC_MSG_CHECKING(for tm_gmtoff in struct tm)
AC_TRY_COMPILE([#include <time.h>], [struct tm tm; tm.tm_gmtoff=0;], AC_DEFINE(HAVE_TM_GMTOFF) AC_MSG_RESULT(yes), AC_MSG_RESULT(no))

AC_MSG_CHECKING(for tm_tzadj in struct tm)
AC_TRY_COMPILE([#include <time.h>], [struct tm tm; tm.tm_tzadj=0;], AC_DEFINE(HAVE_TM_TZADJ) AC_MSG_RESULT(yes), AC_MSG_RESULT(no))

AC_MSG_CHECKING(for gettid)
AC_TRY_COMPILE([
#include <sys/types.h>
#include <sys/syscall.h>
], [uint tid=syscall(__NR_gettid);], AC_DEFINE(HAVE_GETTID) AC_MSG_RESULT(yes), AC_MSG_RESULT(no))

AC_MSG_CHECKING(for TLS)
AC_TRY_COMPILE([__thread int i=0;], [], AC_DEFINE(HAVE_TLS) AC_MSG_RESULT(yes), AC_MSG_RESULT(no))

AC_MSG_CHECKING(for 0..9999 year date range support)
AC_TRY_RUN([
#include <string.h>
#include <time.h>

int main() {
	struct tm tmL,tmH;
	memset(&tmL, 0, sizeof(tmL));
	memset(&tmH, 0, sizeof(tmH));
	tmL.tm_year=-1900;
	tmL.tm_mon=-1;
	tmH.tm_year=9999-1900;
	tmH.tm_mon=12-1;
	tmH.tm_mday=31;
	return mktime(&tmL)==-1 || mktime(&tmH)==-1;
}
], AC_DEFINE(PA_DATE64) AC_MSG_RESULT(yes), AC_MSG_RESULT(no), AC_MSG_RESULT(no))

dnl Output header and makefiles
AH_TEMPLATE([HAVE_TIMEZONE],[Define if you have timezone external variable in <time.h>])
AH_TEMPLATE([HAVE_TM_GMTOFF],[Define if you have tm_gmtoff member of tm structure in <time.h>])
AH_TEMPLATE([HAVE_TM_TZADJ],[Define if you have tm_tzadj member of tm structure in <time.h>])
AH_TEMPLATE([HAVE_GETTID],[Define if you have gettid()])
AH_TEMPLATE([HAVE_TLS],[Define if you have Thread-Local Storage])
AH_TEMPLATE([PA_DATE64],[Define if you have 0..9999 year date range support])

AM_CONFIG_HEADER(src/include/pa_config_auto.h)

AC_OUTPUT(
	Makefile 
	src/Makefile 
	src/types/Makefile 
	src/classes/Makefile 
	src/include/Makefile 
	src/main/Makefile 
	src/sql/Makefile 
	src/lib/Makefile 
	src/lib/gd/Makefile 
	src/lib/smtp/Makefile 
	src/lib/gc/Makefile 
	src/lib/gc/include/Makefile
	src/lib/pcre/Makefile 
	src/lib/cord/Makefile
	src/lib/cord/include/Makefile
	src/lib/cord/include/private/Makefile
	src/lib/md5/Makefile
	src/lib/sdbm/Makefile
	src/lib/sdbm/pa-include/Makefile
	src/lib/json/Makefile
	src/lib/curl/Makefile
	src/lib/punycode/Makefile
	src/lib/memcached/Makefile
	src/targets/Makefile
	src/targets/cgi/Makefile
	src/targets/apache/Makefile
	src/targets/isapi/Makefile
	etc/Makefile
	etc/parser3.charsets/Makefile
	etc/auto.p)
