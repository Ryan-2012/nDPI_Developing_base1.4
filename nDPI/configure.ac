AC_INIT([nDPI], [1.4.0])
AC_CONFIG_SRCDIR([src/include/ndpi_protocols.h])

AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],[output debug log (default is no)])],
    [if test "$enableval" == "yes"; then
        AC_DEFINE([DEBUG],[1],[Define if --enable-debug])
        AC_DEFINE([NDPI_ENABLE_DEBUG_MESSAGES],[1],[Define if --enable-debug])
    fi
    ])


AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_LIBTOOL

AC_PROG_CC
AC_PROG_INSTALL

LT_INIT

AC_CHECK_HEADERS([netinet/in.h stdint.h stdlib.h string.h unistd.h])

AC_CONFIG_FILES([Makefile
		src/lib/Makefile
		src/include/Makefile
		libndpi.pc
		])

AC_CONFIG_HEADERS(config.h)

AC_OUTPUT
