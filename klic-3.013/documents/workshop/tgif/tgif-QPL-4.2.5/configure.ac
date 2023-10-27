#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(tgif, 4.2.5, bill.cheng@acm.org)
AC_CONFIG_SRCDIR([wb_seg.c])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([X11], [XOpenDisplay])
AC_CHECK_LIB([Xt], [XtInitialize])
AC_CHECK_LIB([socket], [socket])
AC_CHECK_LIB([nsl], [inet_addr])
AC_CHECK_LIB([dl], [dlsym])
AC_CHECK_LIB([idn], [stringprep_convert])
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB([z], [deflate])
AC_CHECK_LIB([pthread], [pthread_create])

# Checks for header files.
AC_PATH_X
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h libintl.h netdb.h netinet/in.h stdint.h stdlib.h string.h sys/file.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_LSTAT
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRTOD
AC_CHECK_FUNCS([ftime getcwd gethostbyname gethostname gettimeofday memmove memset mkdir pow putenv select setlocale socket sqrt strchr strpbrk strrchr strstr strtol])

AC_CONFIG_FILES([Makefile])
# BC: skipping AC_CONFIG_SUBDIRS([rmcast])
AC_OUTPUT
