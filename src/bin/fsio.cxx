// This file is part of fsio, see <https://qu1x.org/fsio>.
// 
// Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
// 
// fsio is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License version 3
// as published by the Free Software Foundation on 19 November 2007.
// 
// fsio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with fsio. If not, see <https://www.gnu.org/licenses>.

#include "../lib/fsio.hxx"

#define FSIO_MODE 0640
#define FSIO_BUFS 16384

#define STR_EXPAND(x) #x
#define STR(x) STR_EXPAND(x)

#include <memory>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cerrno>

#include <getopt.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

const char* INFO =
PACKAGE_STRING "  <" PACKAGE_URL ">\n"
"\n"
"Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>\n"
"\n"
PACKAGE_TARNAME" is free software: you can redistribute it and/or modify\n"
"it under the terms of the GNU Affero General Public License version 3\n"
"as published by the Free Software Foundation on 19 November 2007.\n"
"\n"
PACKAGE_TARNAME " is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
"GNU Affero General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU Affero General Public License\n"
"along with " PACKAGE_TARNAME ". If not, see <https://www.gnu.org/licenses>.";

const char* HELP =
"fsio - Feedback Synchronized I/O\n"
"\n"
"fsio [OPTION]... [--] FSIO MODE FILE [DUMP]\n"
"  -e, --eof EOF          Stop FILE transfer after EOF bytes.\n"
"  -b, --buf BUF (" STR(FSIO_BUFS) ")  "
                         "Use BUF multiples of FSIO width as FILE buffer.\n"
"  -s, --show buf|eof     Show statistics after each BUF or at EOF only.\n"
"  -F, --no-feed          Get without feeding data back.\n"
"  -B, --no-back          Put without getting data back.\n"
#if FSIO_WAIT
"  -p, --poll POLL (" STR(FSIO_POLL) ")    "
                         "Use polling interval of POLL nanoseconds.\n"
#endif
"  -h, --help             Print this help.\n"
"  -v, --version          Print version information.\n"
"\n"
"  Map FSIO channel and transfer FILE. Either get FILE from\n"
"  or put FILE to FSIO with MODE of get|put, respectively.\n"
"  Optionally dump statistics to DUMP.\n"
"\n"
"For complete documentation, run `man fsio'.";

const char* HINT =
"\n"
"Try `fsio --help' for more information.";

class opts {
public:
	int help, info;
	enum mode_t {
		dry,
		get,
		put,
	} mode;
	enum show_t {
		non,
		buf,
		eof,
	} show;
	std::string fsio;
	std::string file;
	std::string dump;
	std::size_t eofs;
	std::size_t bufs;
	std::size_t poll;
	int feed, back;
	opts() {
		help = info = 0;
		mode = dry;
		show = non;
		eofs = 0;
		bufs = FSIO_BUFS;
		poll = FSIO_POLL;
		feed = back = 1;
	}
	auto parse(int& argc, char**& argv) -> bool;
};

auto opts::parse(int& argc, char**& argv) -> bool {
	opterr = 0;
	const char* opts = "-:e:b:s:FB"
#if FSIO_WAIT
		"p:"
#endif
		"hv";
	option optl[] = {
		{ "eof", required_argument, nullptr, 'e' },
		{ "buf", required_argument, nullptr, 'b' },
		{ "show", required_argument, nullptr, 's' },
		{ "no-feed", no_argument, nullptr, 'F' },
		{ "no-back", no_argument, nullptr, 'B' },
#if FSIO_WAIT
		{ "poll", required_argument, nullptr, 'p' },
#endif
		{ "help", no_argument, nullptr, 'h' },
		{ "version", no_argument, nullptr, 'v' },
		{ nullptr, 0, nullptr, 0 }
	};
	int optv;
	auto fail = [&optv, &argv] (std::string msg) {
		msg += " option '";
		if (optv)
			msg.append("-").push_back((char)optv);
		else
			msg += argv[optind-1];
		msg += "'";
		throw msg;
	};
	auto to_number = [&fail] (const std::string& str, int base) {
		std::size_t num = 0;
		try {
			num = std::stoull(str, 0, base);
		}
		catch (const std::invalid_argument&) {
			fail("Invalid argument for");
		}
		catch (const std::out_of_range&) {
			fail("Too large argument for");
		}
		return num;
	};
	int args = 0;
	while (-1 != (optv = getopt_long(argc, argv, opts, optl, nullptr))) {
		switch (optv) {
			case 'e': eofs = to_number(optarg, 10); break;
			case 'b': bufs = to_number(optarg, 10); break;
			case 's':
				if (!std::strcmp(optarg, "buf"))
					show = buf;
				else
				if (!std::strcmp(optarg, "eof"))
					show = eof;
				else
					fail("Invalid argument for");
				break;
			case 'F': feed = 0; break;
			case 'B': back = 0; break;
#if FSIO_WAIT
			case 'p': poll = to_number(optarg, 10); break;
#endif
			case 'h': help = 1; break;
			case 'v': info = 1; break;
			case  0 : break;
			case  1 :
				switch (++args) {
					case 1: fsio = optarg; break;
					case 2:
						if (!std::strcmp(optarg, "get"))
							mode = get;
						else
						if (!std::strcmp(optarg, "put"))
							mode = put;
						else
							throw std::string("Invalid MODE");
						break;
					case 3: file = optarg; break;
					case 4: dump = optarg; break;
					default: throw std::string("Too many arguments");
				}
				break;
			case ':': optv = optopt; fail("Missing argument for"); break;
			case '?': optv = optopt; fail("Unknown"); break;
			default: throw std::string("Unknown error");
		}
	}
	if (!help && !info)
		switch (args) {
			case 0: throw std::string("Missing FSIO"); break;
			case 1: throw std::string("Missing MODE"); break;
			case 2: throw std::string("Missing FILE"); break;
		}
	return help || info;
}

auto fmt_p100(std::size_t val, std::size_t ref, std::string suf = " %")
-> std::string {
	std::size_t pct = 0;
	if (ref)
		pct = std::round((double)val / (double)ref * 100.0);
	auto str = std::to_string(pct);
	return std::string(3 - str.size(), ' ') + str + suf;
}

auto get_time() -> timespec {
	timespec time;
	if (-1 == clock_gettime(CLOCK_MONOTONIC, &time))
		throw std::string("Cannot get time: ") + std::strerror(errno);
	return time;
}

auto int_time(const timespec& time) -> long {
	return time.tv_sec + (time.tv_nsec + 500'000'000) / 1'000'000'000;
}

auto flt_time(const timespec& time) -> double {
	return (double)time.tv_sec + (double)time.tv_nsec / 1'000'000'000.0;
}

auto operator-=(timespec& time, const timespec& last) -> timespec& {
	time.tv_sec -= last.tv_sec;
	if (time.tv_nsec < last.tv_nsec) {
		time.tv_nsec += 1'000'000'000 - last.tv_nsec;
		--time.tv_sec;
	}
	else
		time.tv_nsec -= last.tv_nsec;
	return time;
}

auto fmt_time(const timespec& time) -> std::string {
	std::size_t s = int_time(time), m = s / 60, h = m / 60;
	s -= m * 60;
	m -= h * 60;
	if (h > 99)
		return "--:--:--";
	char str[9];
	int len = std::snprintf(str, sizeof str, "%02zd:%02zd:%02zd", h, m, s);
	if (len != sizeof str - 1)
		throw std::string("Cannot format time");
	return std::string(str, len);
}

auto fmt_size(std::uint64_t size, std::uint8_t precision = 2) -> std::string {
	static const char* prefix = "kMGTPE";
	static const std::uint64_t pow[] = {
		1'000,
		1'000'000,
		1'000'000'000,
		1'000'000'000'000,
		1'000'000'000'000'000,
		1'000'000'000'000'000'000,
	};
	char str[23];
	int len = 0;
	std::uint8_t leading = precision + 4;
	if (!precision)
		--leading;
	if (size < pow[0])
		len = std::snprintf(str, sizeof str, "%*zd B", leading + 1,
			(std::size_t)size);
	else {
		auto exp = std::size_t(std::log(size) / 6.90775527898213682);
		len = std::snprintf(str, sizeof str, "%*.*f %cB", leading, precision,
			(double)size / (double)pow[exp - 1], prefix[exp - 1]);
	}
	if (!(0 < len && len < (int)sizeof str))
		throw std::string("Cannot format byte size");
	return std::string(str, len);
}

auto fmt_rate(
	std::size_t size, timespec time,
	std::uint8_t precision = 2
) -> std::string {
	if (!time.tv_sec && !time.tv_nsec) {
		size = 0;
		time.tv_sec = 1;
	}
	return fmt_size(std::round(
		(double)size / flt_time(time)), precision) + "/s";
}

class term {
	static int halt_status;
	static int halt_single;
	static int halt_double;
	static bool wsch;
	static struct winsize ws;
	static struct termios tp;
public:
	static bool endl;
	static void halt_handler(int) {
		if (halt_status)
			exit(halt_status = halt_double);
		halt_status = halt_single;
	}
	static void wsch_handler(int) { wsch = true; }
	static struct winsize size() {
		if (wsch) {
			wsch = false;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
		}
		return ws;
	}
	static auto halt() -> int { return halt_status; }
	term() {
		struct sigaction sa;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sa.sa_handler = halt_handler;
		sigaction(SIGINT, &sa, nullptr);
		sa.sa_handler = wsch_handler;
		sigaction(SIGWINCH, &sa, nullptr);
		tcgetattr(STDIN_FILENO, &tp);
		auto tp = term::tp;
		tp.c_lflag &= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp);
	}
	~term() {
		if (endl && halt_status == halt_double)
			std::cout << std::endl;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp);
	}
} _term;

int term::halt_status = 0;
int term::halt_single = 66;
int term::halt_double = 67;
bool term::wsch = true;
bool term::endl = false;
struct winsize term::ws;
struct termios term::tp;

void stat(
	std::ofstream& dump, fsio& port,
	opts::mode_t mode = opts::dry, bool show = false,
	std::size_t osum = 0, std::size_t eofs = 0,
	std::size_t bufs = 0, std::size_t size = 0
) {
	auto time = get_time(), span = time;
	static timespec zero = { 0, 0 }, last = time;
	if (!zero.tv_sec)
		zero = time;
	span -= last;
	last = time;
	time -= zero;
	if (dump.is_open() && size)
		try {
			dump << (double)size / flt_time(span);
#if FSIO_STAT
			static std::vector<std::size_t> sv;
			port.stat(sv);
			for (auto poll: sv)
				dump << " " << (double)poll / (double)bufs;
#else
			bufs *= port.size();
#endif
			dump << "\n";
		}
		catch (const std::ofstream::failure&) {
			throw std::string("Cannot dump statistics: ") +
				std::strerror(errno);
		}
	if (!show)
		return;
	std::size_t min = 40, max = term::size().ws_col;
	if (min > max)
		return;
	auto mode_str = mode == opts::get ? "GET" : "PUT";
	auto osum_str = fmt_size(osum);
	auto time_str = fmt_time(time);
	auto rate_str = size ? fmt_rate(size, span) : fmt_rate(osum, time);
	std::cout << "\r" << mode_str << ": " << rate_str
		<< " (" << osum_str << " in " << time_str << ")";
	if (eofs && (min = 75) <= max) {
		auto eofs_str = fmt_size(eofs);
		auto p100_str = fmt_p100(osum, eofs);
		auto time_str = fmt_time({ (time_t)std::round((double)(eofs - osum) /
			(double)(size ? size : osum) * flt_time(size ? span : time)), 0 });
		std::cout << std::string(max - min + 1, ' ') << "ETA: " << time_str
			<< " (" << p100_str << " of " << eofs_str << ")";
	}
	std::cout << std::flush;
	term::endl = true;
}

auto main(int argc, char** argv) -> int {
	try {
		opts args = opts();
		try {
			if (args.parse(argc, argv)) {
				if (args.help)
					std::cout << HELP << std::endl;
				else
				if (args.info)
					std::cout << INFO << std::endl;
				return 0;
			}
		}
		catch (const std::string& err) {
			std::cerr << "Invalid usage: " << err << std::endl;
			std::cerr << HINT << std::endl;
			return 64;
		}
		std::ofstream dump;
		dump.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		try {
			if (args.dump.size())
				dump.open(args.dump);
		}
		catch (const std::ofstream::failure&) {
			throw "Cannot open " + args.dump + ": " + std::strerror(errno);
		}
		auto port = fsio(args.fsio, args.feed, args.back, args.poll);
		const std::size_t port_size = port.size();
		const std::size_t data_size = args.bufs * port.size();
		auto data = std::make_unique<char[]>(data_size);
		char* iptr = data.get(), * const iend = iptr + data_size;
		const char* optr = iptr, * const oend = iend;
		std::size_t isum = 0, osum = 0;
		int fd = -1;
		if (args.mode == opts::get) {
			if ((fd = creat(args.file.data(), FSIO_MODE)) == -1)
				throw "Cannot open " + args.file + ": " + std::strerror(errno);
			for (bool ieof = false; optr != iptr || !ieof;) {
				if (term::halt())
					break;
				if (iptr != iend && !ieof) {
					std::size_t size = port_size;
					if (args.eofs) {
						std::size_t left = args.eofs - isum;
						if (size > left)
							size = left;
					}
					port.get(iptr);
					if (!isum)
						stat(dump, port);
					iptr += size;
					isum += size;
					if (args.eofs == isum)
						ieof = true;
				}
				std::size_t left = iptr - optr;
				if (left) {
					ssize_t size = write(fd, optr, left);
					if (size < 0) {
						if (errno == EINTR) {
							stat(dump, port, args.mode, args.show == opts::buf,
								osum, args.eofs, args.bufs, data_size);
							continue;
						}
						else
							throw "Cannot write " + args.file + ": " +
								std::strerror(errno);
					}
					optr += size;
					osum += size;
					if (optr == oend) {
						optr = iptr = data.get();
						stat(dump, port, args.mode, args.show == opts::buf,
							osum, args.eofs, args.bufs, data_size);
					}
				}
			}
		}
		else
		if (args.mode == opts::put) {
			if ((fd = open(args.file.data(), O_RDONLY)) == -1)
				throw "Cannot open " + args.file + ": " + std::strerror(errno);
			if (!args.eofs) {
				struct stat buf;
				fstat(fd, &buf);
				args.eofs = buf.st_size;
			}
			for (bool ieof = false; optr != iptr || !ieof;) {
				if (term::halt())
					break;
				if (iptr != iend && !ieof) {
					std::size_t free = iend - iptr;
					if (args.eofs) {
						std::size_t left = args.eofs - isum;
						if (free > left)
							free = left;
					}
					ssize_t size = read(fd, iptr, free);
					if (size < 0) {
						if (errno == EINTR) {
							stat(dump, port, args.mode, args.show == opts::buf,
								osum, args.eofs, args.bufs, data_size);
							continue;
						}
						else
							throw "Cannot read " + args.file + ": " +
								std::strerror(errno);
					}
					iptr += size;
					isum += size;
					if (!size || args.eofs == isum)
						ieof = true;
				}
				std::size_t size = iptr - optr;
				if (size && (size >= port_size || ieof)) {
					if (size > port_size)
						size = port_size;
					port.put(optr);
					if (!osum)
						stat(dump, port);
					optr += size;
					osum += size;
					if (optr == oend) {
						optr = iptr = data.get();
						stat(dump, port, args.mode, args.show == opts::buf,
							osum, args.eofs, args.bufs, data_size);
					}
				}
			}
		}
		stat(dump, port, args.mode, args.show != opts::non, osum, args.eofs);
		if (term::endl)
			std::cout << std::endl;
	}
	catch (const std::string& err) {
		if (term::endl)
			std::cout << std::endl;
		std::cerr << err << std::endl;
		return 65;
	}
	catch (const std::exception& err) {
		if (term::endl)
			std::cout << std::endl;
		std::cerr << err.what() << std::endl;
		return 113;
	}
	return term::halt();
}
