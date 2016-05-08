// This file is part of libfsio, see <https://qu1x.org/libfsio>.
// 
// Copyright (c) 2016 Rouven Spreckels <n3vu0r@qu1x.org>
// 
// libfsio is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License version 3
// as published by the Free Software Foundation on 19 November 2007.
// 
// libfsio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
// 
// You should have received a copy of the GNU Affero General Public License
// along with libfsio. If not, see <https://www.gnu.org/licenses>.

#include "fsio.hxx"
#include "fsio_map.hxx"

#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

std::string fsio::text(std::string file) {
	std::string dst;
	try {
		std::ifstream src(file.data(), std::ios::in | std::ios::binary);
		src.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		src.seekg(0, std::ios::end);
		dst.resize(src.tellg());
		src.seekg(0, std::ios::beg);
		src.read(&dst.front(), dst.size());
	}
	catch (std::ifstream::failure&) {
		throw "Cannot read file " + file + ": " + std::strerror(errno);
	}
	return dst;
}

void fsio::load(pugi::xml_document& tree, std::string file, std::string root) {
	auto xml = text(file);
	auto res = tree.load(xml.data());
	if (!res) {
		std::string err(res.description());
		if (res.offset) {
			std::string msg(xml.data() + res.offset);
			msg = msg.substr(0, msg.find_first_of('\n') - 1);
			err += " at character " + std::to_string(res.offset)
				+ " [..." + msg + "...]";
		}
		throw err;
	}
	if (root.size() && !tree.child(root.data()))
		throw "Missing root node " + root;
}

auto fsio::make(int fd, pugi::xml_node node) -> std::unique_ptr<map> {
	std::size_t i = std::strtoul(node.attribute("i").value(), nullptr, 0);
	std::size_t o = std::strtoul(node.attribute("o").value(), nullptr, 0);
	bool d = node.attribute("o").value()[0] == '+';
	switch (node.attribute("width").as_uint()) {
		case 1: return std::make_unique<fsio_map<std::uint8_t>>(fd, i, o, d);
		case 2: return std::make_unique<fsio_map<std::uint16_t>>(fd, i, o, d);
		case 4: return std::make_unique<fsio_map<std::uint32_t>>(fd, i, o, d);
		case 8: return std::make_unique<fsio_map<std::uint64_t>>(fd, i, o, d);
		default: throw std::string("Unsupported map width");
	}
}

void fsio::make(pugi::xml_node node) {
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd == -1)
		throw std::string("Cannot open memory: ") + std::strerror(errno);
	hs = make(fd, node.child("hs"));
	for (auto item: node.children("fs"))
		fs.emplace_back(make(fd, item));
	if (close(fd) == -1)
		throw std::string("Cannot close memory: ") + std::strerror(errno);
	fs_size = 0;
	for (auto& io: fs)
		fs_size += io->size();
}

fsio::fsio(pugi::xml_node node, bool feed, bool back, std::size_t poll):
fsio(feed, back, poll) {
	make(node);
}

fsio::fsio(std::string file, bool feed, bool back, std::size_t poll):
fsio(feed, back, poll) {
	pugi::xml_document tree;
	load(tree, file);
	make(tree.first_child());
}

#if FSIO_WAIT
void fsio::wait() const {
	if (poll) {
		timespec req = { 0, (long)poll };
		nanosleep(&req, 0);
	}
}
#endif

void fsio::feed() const {
	auto feed = [this] () {
		for (auto& io: fs)
			io->feed();
	};
	for (hs->wave(), feed(); !hs->back(); wait())
		feed();
}

void fsio::back() const {
	for (bool back = false; !back; wait()) {
		back = true;
		for (auto& io: fs)
			if (!io->back())
				back = false;
	}
	wave();
}

void fsio::stat(std::vector<std::size_t>& sv) const {
	sv.clear();
	sv.reserve(fs.size() + 1);
	sv.push_back(hs->stat());
	for (auto& io: fs)
		sv.push_back(io->stat());
}

auto fsio::get(void* data) const -> std::size_t {
	feed_data ? feed() : wave();
	for (auto& io: fs)
		data = (char*)data + io->get(data);
	return fs_size;
}

auto fsio::put(const void* data) const -> std::size_t {
	for (auto& io: fs)
		data = (const char*)data + io->put(data);
	back_data ? back() : wave();
	return fs_size;
}
