AC_INIT([amjstr], [0.1.0], [bug-automake@gnu.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_LEX
AC_CONFIG_HEADERS([config.h])
AM_PROG_AR
AC_PROG_RANLIB
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 include/Makefile
 test/Makefile
])
AC_OUTPUT
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([nlohmann/json.hpp],
[], [AC_MSG_ERROR([required libraries are missing])])
AC_LANG_POP([C++])
    
