# paf@design.ru
# included in configure.ac

AC_DEFUN([PA_CHECK_MATH_FUNC_ONE_ARG],[
AC_MSG_CHECKING(for (maybe built-in) math function $1)
AC_TRY_COMPILE([
#ifdef HAVE_MATH_H
#       include <math.h>
#endif
],[
        double result=$1(1.6);
],
[AC_MSG_RESULT(yes)
$2],
[AC_MSG_RESULT(no)
$3])
]
)


AC_DEFUN([PA_CHECK_MATH_FUNCS_ONE_ARG],[
AC_FOREACH([AC_Func], [$1],
  [AH_TEMPLATE(AS_TR_CPP([HAVE_]AC_Func),
               [Define to 1 if you have the `]AC_Func[' (maybe built-in) math function function.])])dnl

for pa_func in $1
do
PA_CHECK_MATH_FUNC_ONE_ARG($pa_func,
	      [AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$pa_func]) $2)],
              [$3])dnl
done
])


AC_DEFUN([PA_CHECK_SIGSETJMP],[
pa_func=sigsetjmp
AC_MSG_CHECKING(for (maybe built-in) function $pa_func)
AC_TRY_COMPILE([
#ifdef HAVE_SETJMP_H
#	include <setjmp.h>
#endif
],[
        $pa_func(0,0);
],
[AC_MSG_RESULT(yes)
AC_DEFINE_UNQUOTED(AS_TR_CPP([HAVE_$pa_func]))
],
[AC_MSG_RESULT(no)
])
]
)
