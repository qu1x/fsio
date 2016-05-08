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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstring>
#include <cerrno>

const char* HELP =
"fsio-tvgen - Test Vector Generator\n"
"\n"
"fsio-tvgen [ROWS COLS SIZE]\n"
"\n"
"  Generate test vector ROWSxCOLSxSIZE.tv of ROWS rows.\n"
"  Each row has COLS cols of SIZE-byte with values of current row number.\n"
"\n"
"  With no arguments, print this help.\n"
"\n"
"For complete documentation, run `man fsio-tvgen'.";

const char* HINT =
"\n"
"Try `fsio-tvgen --help' for more information.";

template<typename T>
void gen(std::string rows_str, std::string cols_str, std::string size_str) {
	T rows = std::stoull(rows_str);
	T cols = std::stoull(cols_str);
	std::ofstream tv;
	tv.exceptions(std::ofstream::failbit | std::ofstream::badbit);
	tv.open(rows_str + "x" + cols_str + "x" + size_str + ".tv");
	for (T row = 0; row < rows; ++row)
		for (T col = 0; col < cols; ++col)
			tv.write((const char*)&row, sizeof row);
}

int main(int argc, char** argv) {
	try {
		if (argc == 1) {
			std::cout << HELP << std::endl;
			return 0;
		}
		else
		if (argc != 4)
			throw std::string("Invalid count of arguments");
		std::string rows(argv[1]), cols(argv[2]), size(argv[3]);
		try {
			switch (std::stoull(size)) {
				case 1: gen<std:: uint8_t>(rows, cols, size); break;
				case 2: gen<std::uint16_t>(rows, cols, size); break;
				case 4: gen<std::uint32_t>(rows, cols, size); break;
				case 8: gen<std::uint64_t>(rows, cols, size); break;
				default: throw std::string("SIZE not of 1, 2, 4, or 8");
			}
		}
		catch (const std::invalid_argument&) {
			throw std::string("Invalid argument");
		}
		catch (const std::out_of_range&) {
			throw std::string("Out of range");
		}
	}
	catch (const std::string& err) {
		std::cerr << "Invalid usage: " << err << "\n" << HINT << std::endl;
		return 64;
	}
	catch (const std::ofstream::failure&) {
		std::cerr << "Cannot generate test vector";
		if (errno)
			std::cerr << ": " << std::strerror(errno) << std::endl;
		else
			std::cerr << std::endl;
		return 65;
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		return 113;
	}
	return 0;
}
