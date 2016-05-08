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

#ifndef _FSIO_H
#define _FSIO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory>
#include <vector>
#include <cstddef>

#include <pugixml.hpp>

#ifndef FSIO_FEED
#define FSIO_FEED true
#endif

#ifndef FSIO_BACK
#define FSIO_BACK true
#endif

#ifndef FSIO_STAT
#define FSIO_STAT 0
#endif

#ifndef FSIO_WAIT
#define FSIO_WAIT 0
#endif

#ifndef FSIO_POLL
#define FSIO_POLL 0
#endif

class fsio {
	class map {
	public:
		virtual ~map() {};
		virtual auto size() const -> std::size_t = 0;
		virtual void wave() = 0;
		virtual void feed() = 0;
		virtual auto back() -> bool = 0;
		virtual auto stat() -> std::size_t = 0;
		virtual void own() = 0;
		virtual auto get(void* data) const -> std::size_t = 0;
		virtual auto put(const void* data) -> std::size_t = 0;
	};
	std::size_t fs_size;
	std::unique_ptr<map> hs;
	std::vector<std::unique_ptr<map>> fs;
	static auto make(int fd, pugi::xml_node node) -> std::unique_ptr<map>;
	void make(pugi::xml_node node);
#if FSIO_WAIT
	void wait() const;
#else
	void wait() const { __asm__ __volatile__ ("" ::: "memory"); };
#endif
	void feed() const;
	void back() const;
	void wave() const { for (hs->wave(); !hs->back(); wait()); };
	fsio(bool feed = FSIO_FEED, bool back = FSIO_BACK,
		std::size_t poll = FSIO_POLL)
	: feed_data(feed), back_data(back), poll(poll) {};
public:
	bool feed_data;
	bool back_data;
	std::size_t poll;
	static std::string text(std::string file);
	static void load(pugi::xml_document& tree,
		std::string file, std::string root = "fsio");
	fsio(pugi::xml_node node, bool feed = FSIO_FEED, bool back = FSIO_BACK,
		std::size_t poll = FSIO_POLL);
	fsio(std::string file, bool feed = FSIO_FEED, bool back = FSIO_BACK,
		std::size_t poll = FSIO_POLL);
	void stat(std::vector<std::size_t>& sv) const;
	auto size() const -> std::size_t { return fs_size; };
	void own() const { hs->own(); };
	auto get(void* data) const -> std::size_t;
	auto put(const void* data) const -> std::size_t;
};

#endif
