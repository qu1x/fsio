# This file is part of fsio, see <https://qu1x.org/fsio>.
# 
# Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
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

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libfsio.la

libfsio_ladir = $(includedir)/fsio
libfsio_la_LDFLAGS = -version-info 0:0:0
libfsio_la_LIBADD = -lpugixml
libfsio_la_HEADERS = src/lib/fsio.hxx src/lib/fsio_map.hxx
libfsio_la_SOURCES = $(libfsio_la_HEADERS) src/lib/fsio.cxx

vhddir = $(includedir)/fsio
dist_vhd_HEADERS = $(srcdir)/src/lib/fsio*.vhd

bin_PROGRAMS = fsio fsio-tvgen

fsio_LDADD = libfsio.la
fsio_SOURCES = src/bin/fsio.cxx

fsio_tvgen_SOURCES = src/bin/fsio-tvgen.cxx

bash_completiondir = $(sysconfdir)/bash_completion.d
dist_bash_completion_DATA = $(srcdir)/src/bash_completion/*

dist_man_MANS = $(srcdir)/src/man/fsio*.1
