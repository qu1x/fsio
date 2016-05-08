> This file is part of fsio, see <https://qu1x.org/fsio>.
> 
> Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
> 
> fsio is free software: you can redistribute it and/or modify
> it under the terms of the GNU Affero General Public License version 3
> as published by the Free Software Foundation on 19 November 2007.
> 
> fsio is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
> GNU Affero General Public License for more details.
> 
> You should have received a copy of the GNU Affero General Public License
> along with fsio. If not, see <https://www.gnu.org/licenses>.

fsio
====

Feedback Synchronized I/O

Installation
============

Either getting a release
------------------------

1. Download stable source distribution tarball.

		wget https://qu1x.org/file/fsio-1.0.0.tar.xz

2. Extract and enter.

		tar -xJf fsio-1.0.0.tar.xz
		cd fsio-1.0.0

Or getting a snapshot
---------------------

Required dependencies:

		git autoreconf libtool

1. Clone repository.

		git clone https://github.com/qu1x/fsio.git

2. Enter and generate latest source distribution.

		cd fsio
		./bootstrap

Installing one of them
----------------------

Required dependencies:

		build-essential libpugixml

1. Configure, build, and install.

Two features, `--enable-stat` and `--enable-wait`, can be enabled.

		./configure --sysconfdir=/etc
		make
		sudo make install

2. Keep to uninstall someday.

		sudo make uninstall

Usage
=====

RTFM:

		man fsio
		man fsio-tvgen

Version
=======

fsio-1.0.0 <https://qu1x.org/fsio>

License
=======

GNU Affero General Public License version 3

Authors
=======

* Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>

