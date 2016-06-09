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

		wget https://qu1x.org/file/fsio-1.0.1.tar.xz

2. Extract and enter.

		tar -xJf fsio-1.0.1.tar.xz
		cd fsio-1.0.1

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

Have a look at the package definition and the top-level loopback example:

		less fsio.vhd
		less fsio_top.vhd

Performance Measurements
========================

	SoM: Avnet PicoZed 7Z030
	SoC: Zynq XC7Z030 (-1)
	CLK: 180 MHz
	eOS: Xilinx PetaLinux 2015.4
	CXX: arm-linux-gnueabi-gcc -std=c++14 -O3 (4.9.2)

Write Rate (PS to PL)
---------------------

![Write Rate vs. Channel Width](doc/put-rate.png)

| Maps | Channel Width (B) | Rate (MB/s) | Rate Uncertainty (MB/s) |
| ----:|:----------------- | -----------:|:----------------------- |
|    1 | 4                 |       4.540 | 0.006                   |
|    2 | 8                 |       6.288 | 0.016                   |
|    3 | 12                |       7.231 | 0.008                   |
|    4 | 16                |       7.798 | 0.015                   |
|    5 | 20                |       8.095 | 0.038                   |
|    6 | 24                |       8.452 | 0.009                   |
|    7 | 28                |       8.672 | 0.010                   |
|    8 | 32                |       8.827 | 0.010                   |
|    9 | 36                |       8.986 | 0.016                   |
|   10 | 40                |       9.095 | 0.013                   |
|   11 | 44                |       9.135 | 0.012                   |
|   12 | 48                |       9.203 | 0.015                   |
|   13 | 52                |       9.314 | 0.013                   |
|   14 | 56                |       9.386 | 0.013                   |
|   15 | 60                |       9.452 | 0.013                   |
|   16 | 64                |       9.472 | 0.014                   |
|   17 | 68                |       9.461 | 0.015                   |
|   18 | 72                |       9.480 | 0.015                   |
|   19 | 76                |       9.555 | 0.018                   |
|   20 | 80                |       9.594 | 0.019                   |
|   21 | 84                |       9.630 | 0.017                   |
|   22 | 88                |       9.652 | 0.017                   |
|   23 | 92                |       9.679 | 0.017                   |
|   24 | 96                |       9.687 | 0.018                   |
|   25 | 100               |       9.717 | 0.018                   |
|   26 | 104               |       9.733 | 0.018                   |
|   27 | 108               |       9.737 | 0.019                   |
|   28 | 112               |       9.734 | 0.019                   |
|   29 | 116               |       9.735 | 0.019                   |
|   30 | 120               |       9.734 | 0.020                   |
|   31 | 124               |       9.736 | 0.020                   |
|   32 | 128               |       9.733 | 0.021                   |

Read Rate (PL to PS)
--------------------

![Read Rate vs. Channel Width](doc/get-rate.png)

| Maps | Channel Width (B) | Rate (MB/s) | Rate Uncertainty (MB/s) |
| ----:|:----------------- | -----------:|:----------------------- |
|    1 | 4                 |       0.855 | 0.057                   |
|    2 | 8                 |       1.567 | 0.091                   |
|    3 | 12                |       2.184 | 0.114                   |
|    4 | 16                |       2.722 | 0.123                   |
|    5 | 20                |       3.191 | 0.131                   |
|    6 | 24                |       3.605 | 0.130                   |
|    7 | 28                |       3.979 | 0.126                   |
|    8 | 32                |       4.337 | 0.118                   |
|    9 | 36                |       4.611 | 0.110                   |
|   10 | 40                |       4.894 | 0.102                   |
|   11 | 44                |       5.052 | 0.086                   |
|   12 | 48                |       5.319 | 0.077                   |
|   13 | 52                |       5.507 | 0.063                   |
|   14 | 56                |       5.676 | 0.058                   |
|   15 | 60                |       5.862 | 0.042                   |
|   16 | 64                |       6.049 | 0.030                   |
|   17 | 68                |       6.167 | 0.020                   |
|   18 | 72                |       6.276 | 0.009                   |
|   19 | 76                |       6.405 | 0.008                   |
|   20 | 80                |       6.534 | 0.015                   |
|   21 | 84                |       6.626 | 0.022                   |
|   22 | 88                |       6.738 | 0.026                   |
|   23 | 92                |       6.848 | 0.030                   |
|   24 | 96                |       6.958 | 0.031                   |
|   25 | 100               |       7.017 | 0.031                   |
|   26 | 104               |       7.092 | 0.032                   |
|   27 | 108               |       7.195 | 0.030                   |
|   28 | 112               |       7.266 | 0.027                   |
|   29 | 116               |       7.335 | 0.027                   |
|   30 | 120               |       7.385 | 0.025                   |
|   31 | 124               |       7.438 | 0.025                   |
|   32 | 128               |       7.545 | 0.021                   |

Version
=======

fsio-1.0.1 <https://qu1x.org/fsio>

License
=======

GNU Affero General Public License version 3

Authors
=======

* Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>

