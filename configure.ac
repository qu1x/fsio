# This file is part of fsio, see <https://qu1x.org/fsio>.
# 
# Copyright (c) 2016, 2018 Rouven Spreckels <n3vu0r@qu1x.org>
# 
# fsio is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License version 3
# as published by the Free Software Foundation on 19 November 2007.
# 
# fsio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with fsio. If not, see <https://www.gnu.org/licenses>.

AC_INIT([fsio], [1.0.1],
	[n3vu0r@qu1x.org], [fsio], [https://qu1x.org/fsio])

AC_CONFIG_AUX_DIR([.])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/bin/fsio.cxx])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

LT_INIT([disable-static])

AC_PROG_CXX

AC_LANG([C++])

CXX="$CXX -std=c++14"

AC_CHECK_HEADERS([\
sys/mman.h \
sys/types.h \
sys/stat.h \
fcntl.h \
unistd.h \
time.h \
getopt.h \
signal.h \
termios.h \
sys/ioctl.h], [], [AC_MSG_ERROR([header not found])])

AC_CHECK_FUNCS([\
mmap \
munmap \
open \
close \
getopt_long \
clock_gettime \
exit \
ioctl \
sigemptyset \
sigaction \
tcgetattr \
tcsetattr \
creat \
write \
fstat \
read], [], [AC_MSG_ERROR([function not found])])

AC_CHECK_HEADER([pugixml.hpp], [], [AC_MSG_ERROR([libpugixml not found])])

AC_ARG_ENABLE([stat], AS_HELP_STRING([--enable-stat],
	[increase statistics sacrifizing performance]), [], [enable_stat=no])
AM_CONDITIONAL([FSIO_STAT], [test x$enable_stat = xyes])
AM_COND_IF([FSIO_STAT], [AC_DEFINE([FSIO_STAT], [1], [--enable-stat])])

AC_ARG_ENABLE([wait], AS_HELP_STRING([--enable-wait],
	[decrease CPU usage sacrifizing performance]), [], [enable_wait=no])
AM_CONDITIONAL([FSIO_WAIT], [test x$enable_wait = xyes])
AM_COND_IF([FSIO_WAIT], [AC_DEFINE([FSIO_WAIT], [1], [--enable-wait])])
AM_COND_IF([FSIO_WAIT], 
	[AC_CHECK_FUNCS(nanosleep, [], [AC_MSG_ERROR([function not found])])])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

printf "
$PACKAGE_STRING

stat: $enable_stat
wait: $enable_wait

"
